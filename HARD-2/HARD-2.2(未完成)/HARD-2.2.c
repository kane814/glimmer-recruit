//头文件
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#define WSIZE       8   // 头部或指针字段的字节数（64 位一个字）
#define DSIZE       16  // 双字大小，块与负载的对齐粒度
#define CHUNKSIZE  (1 << 12)       // 扩展堆时的默认字节数
#define MAX_HEAP   (5 * (1 << 20)) // 模拟堆区上限（5MB）
#define MIN_BLK_SIZE (2 * DSIZE)   // 最小块大小（须容纳 hdr + pred + succ + ftr）

#define ALIGN(size) (((size) + (DSIZE - 1)) & ~(DSIZE - 1)) // 向上对齐到 DSIZE 的倍数

#define MAX(x, y)  ((x) > (y) ? (x) : (y))  // 返回较大值


#define PACK(size, prev_alloc, alloc) \
    (((size) & ~0x7) | ((prev_alloc) << 1) | (alloc))  // 将 size、前块分配位、本块分配位合并为一个字
#define PACK_PREV_ALLOC(val, prev_alloc) (((val) & ~0x2) | ((prev_alloc) << 1))  // 只更新 header 中的 prev_alloc 位
#define PACK_ALLOC(val, alloc) (((val) & ~0x1) | (alloc))  // 只更新 header 中的 alloc 位

#define GET(p)        (*(unsigned long *)(p))  // 读地址 p 处的一个字
#define PUT(p, val)   (*(unsigned long *)(p) = (val))  // 向地址 p 处写一个字

#define GET_SIZE(p)       (GET(p) & ~0x7)           // 从 header/footer 取出块大小
#define GET_ALLOC(p)      (GET(p) & 0x1)            // 本块是否已分配
#define GET_PREV_ALLOC(p) ((GET(p) >> 1) & 0x1)     // 物理上前一块是否已分配（已分配块可无 footer）

#define HDRP(bp)  ((char *)(bp) - WSIZE)  // 块头部指针（bp 为有效荷载起始地址）
#define FTRP(bp)  ((char *)(bp) + GET_SIZE(HDRP(bp)) - DSIZE)  // 块尾部指针（主要用于空闲块）

#define GET_PRED(bp)  ((char *)GET(bp))  // 空闲块负载处：前驱空闲块指针
#define SET_PRED(bp, val)  (PUT(bp, (unsigned long)(val)))
#define GET_SUCC(bp)  ((char *)GET((char *)(bp) + WSIZE))  // 空闲块负载 +8：后继空闲块指针
#define SET_SUCC(bp, val)  (PUT((char *)(bp) + WSIZE, (unsigned long)(val)))

#define NEXT_BLKP(bp)  ((char *)(bp) + GET_SIZE(((char *)(bp) - WSIZE)))  // 下一块的有效荷载指针
#define PREV_BLKP(bp)  ((char *)(bp) - GET_SIZE(((char *)(bp) - DSIZE)))  // 前一块的有效荷载指针（经前块 footer）

static char *heap_listp;      // 序言块有效荷载地址，物理堆遍历起点
static char *free_listp;      // 显式空闲链表表头
static char *mem_start_brk;   // 向 OS 预留的堆区起始地址
static char *mem_brk;         // 当前堆顶（mm_sbrk 在此基础上递增）
static char *mem_max_addr;    // 堆区上界（mem_start_brk + MAX_HEAP）

#ifdef DEBUG
#define VERBOSE 1  // 在调试模式下启用调试信息输出
#else
#define VERBOSE 0  // 默认不输出调试信息
#endif

//同part1的调试模式
static void print_heap_blocks(int verbose, const char* func) {
    if (verbose) {
        char *curbp = (char *)heap_listp;
        printf("\n=========================== %s ===========================\n", func);

        while (GET_SIZE(HDRP(curbp)) > 0) {
            printf("Address: %p\n", curbp);
            printf("Size: %lu\n", GET_SIZE(HDRP(curbp)));
            printf("Allocated: %lu\n", GET_ALLOC(HDRP(curbp)));
            printf("\n");
            curbp = NEXT_BLKP(curbp);
        }

        printf("Address: %p\n", curbp); // Should point to the end of the heap
        printf("Size: 0\n");
        printf("Allocated: 1\n");
        printf("=========================== %s ===========================\n", func);
    }
}

/**
 * @brief 初始化内存分配器。
 *
 * 实现上文堆的衍生方式中指定的初始化，初始化全局变量，分好堆1堆2
 */
int mm_init(void);

/**
 * @brief 增加程序的数据空间。
 *
 * @param incr 要增加的字节数。
 *
 * @return 扩展后的内存起始地址；如果扩展失败，则返回 (void *)-1。
 *
 * 实现上文堆的衍生方式中的功能
 */
void *mm_sbrk(int incr);

/**
 * @brief 扩展堆空间。
 *
 * @param words 要扩展的字数。
 *
 * @return 返回新扩展区域的起始地址；如果扩展失败，则返回 NULL。
 *
 * extend_heap函数是对mem_sbrk的一层封装，接收的参数是要分配的字数
 * 在堆初始化以及malloc找不到合适内存块时使用
 * 它首先对请求大小进行地址对齐，然后调用mem_sbrk获取空间
 */
static void *extend_heap(size_t words);

/**
 * @brief 查找适合分配的空闲块（首次适应）。
 *
 * @param asize 请求的块大小（以字节为单位）。
 *
 * @return 返回找到的适合的空闲块的指针；如果没有找到合适的空闲块，则返回 NULL。
 *
 * 该函数在空闲链表中查找第一个合适的块，以满足请求的大小。
 */
static void *find_fit_first(size_t asize){
    void* find=free_listp;
    while(find!=NULL){
        if(GET_SIZE(HDRP(find))>=asize){
            break;
        }
        find=GET_SUCC(find);
    }
    return find;
}

//类似find_fit_first
static void *find_fit_best(size_t asize)
{
    size_t best = MAX_HEAP;
    void *best_bp = NULL;

    for (void *find = free_listp; find != NULL; find = GET_SUCC(find)) {
        size_t size = GET_SIZE(HDRP(find));
        if (size >= asize && size < best) {
            best = size;
            best_bp = find;
        }
    }
    return best_bp;
}

/**
 * @brief 将指定的块放置到分配状态。
 *
 * @param bp 要分配的块的起始地址。
 * @param asize 请求的块大小（以字节为单位）。
 *
 * 该函数将指定的块标记为已分配，并更新块的大小信息。
 */
static void place(void *bp, size_t asize)
{
    size_t size = GET_SIZE(HDRP(bp));
    size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    delete_from_free_list(bp);
    if (size - asize >= MIN_BLK_SIZE) {
        // 已分配块
        PUT(HDRP(bp), PACK(asize, prev_alloc, 1));
        // 剩余空闲块
        void *back = NEXT_BLKP(bp);
        PUT(HDRP(back), PACK(size - asize, 1, 0));
        PUT(FTRP(back), PACK(size - asize, 1, 0));
        // 更新剩余块之后的物理块
        void *after = NEXT_BLKP(back);
        PUT(HDRP(after), PACK_PREV_ALLOC(GET(HDRP(after)), 0));
        // 将新空闲块加入链表
        add_to_free_list(back);
    } else {
        // 整块分配，不分割
        PUT(HDRP(bp), PACK(size, prev_alloc, 1));
        void *after = NEXT_BLKP(bp);
        PUT(HDRP(after), PACK_PREV_ALLOC(GET(HDRP(after)), 1));
    }
}

/**
 * @brief 合并相邻的空闲块。
 *
 * @param bp 要合并的块的起始地址。
 *
 * @return 合并后的块的起始地址。
 *
 * 该函数合并当前块与其相邻的空闲块，以减少内存碎片并增加大块的可用内存。
 */
static void *coalesce(void *bp)
{
    size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    void *next = NEXT_BLKP(bp);
    size_t next_alloc = GET_ALLOC(HDRP(next));
    size_t size = GET_SIZE(HDRP(bp));
    // 情况 1：前后都已分配
    if (prev_alloc && next_alloc) {
        void *after = NEXT_BLKP(bp);
        PUT(HDRP(after),
            PACK_PREV_ALLOC(GET(HDRP(after)), 0));
        return bp;
    }
    // 情况 2：前已分配，后空闲
    if (prev_alloc && !next_alloc) {
        delete_from_free_list(next);
        size += GET_SIZE(HDRP(next));

        PUT(HDRP(bp), PACK(size, prev_alloc, 0));
        PUT(FTRP(bp), PACK(size, prev_alloc, 0));
    }
    //情况3或4:前空闲
    else {
        void *prev = PREV_BLKP(bp);
        size_t prev_prev_alloc = GET_PREV_ALLOC(HDRP(prev));

        delete_from_free_list(prev);
        size += GET_SIZE(HDRP(prev));
        // 情况 4：前后都空闲
        if (!next_alloc) {
            delete_from_free_list(next);
            size += GET_SIZE(HDRP(next));
        }
        bp = prev;
        PUT(HDRP(bp), PACK(size, prev_prev_alloc, 0));
        PUT(FTRP(bp), PACK(size, prev_prev_alloc, 0));
    }
    void *after = NEXT_BLKP(bp);
    PUT(HDRP(after), PACK_PREV_ALLOC(GET(HDRP(after)), 0));
    return bp;
}

/**
 * @brief 从空闲链表中移除指定的空闲块。
 *
 * @param bp 要移除的空闲块的起始地址。
 *
 * 该函数将指定的空闲块从空闲链表中移除，以便将其标记为已分配。
 */
static void delete_from_free_list(void *bp){
    void* pred=GET_PRED(bp);
    void* succ=GET_SUCC(bp);
    if(pred!=NULL){
        SET_SUCC(pred,succ);
    }else{
        free_listp=succ;
    }
    if(succ!=NULL){
        SET_PRED(succ,pred);
    }
}

/**
 * @brief 将指定的空闲块插入到空闲链表中。
 *
 * @param bp 要插入的空闲块的起始地址。
 *
 * 该函数用头插法将指定的空闲块插入到空闲链表中，以便将其标记为可用的空闲块。
 */
static void add_to_free_list(void *bp){
    void* head=free_listp;
    SET_PRED((bp),NULL);
    SET_SUCC((bp),head);
    if(head!=NULL){
        SET_PRED((head),bp);
    }
    free_listp=bp;
}

/**
 * @brief 分配内存块。
 *
 * @param size 请求的内存块大小（以字节为单位）。该大小包括块头和块尾所需的空间。
 *
 * @return 返回分配的内存块的起始地址；如果分配失败，则返回 NULL。
 *
 * 该函数根据请求的大小在堆中查找适合的空闲块，如果找到合适的块则分配该块，并返回其起始地址。如果没有足够的空间，则可能会扩展堆或返回 NULL。
 */
void *mm_malloc(size_t size)
{
    if (size == 0)
        return NULL;
    size_t asize = ALIGN(size + WSIZE);
    if (asize < MIN_BLK_SIZE)
        asize = MIN_BLK_SIZE;
    void *bp = find_fit_first(asize);
    if (bp == NULL) {
        size_t extend_size =(asize > CHUNKSIZE) ? asize : CHUNKSIZE;
        bp = extend_heap(extend_size / WSIZE);
        if(bp == NULL)
            return NULL;
    }
    place(bp, asize);
    return bp;
}


/**
 * @brief 释放内存块。
 *
 * @param bp 要释放的内存块的起始地址。
 *
 * 该函数将指定的内存块标记为已释放，并尝试将其合并到相邻的空闲块中，以减少内存碎片。
 */
void mm_free(void *bp)
{
    if (bp == NULL)
        return;
    size_t size = GET_SIZE(HDRP(bp));
    size_t prev_alloc = GET_PREV_ALLOC(HDRP(bp));
    PUT(HDRP(bp), PACK(size, prev_alloc, 0));
    PUT(FTRP(bp), PACK(size, prev_alloc, 0));
    // 告诉后一个物理块：前一块现在空闲
    void *next = NEXT_BLKP(bp);
    PUT(HDRP(next), PACK_PREV_ALLOC(GET(HDRP(next)), 0));
    // 合并，最后只插入一次
    bp = coalesce(bp);
    add_to_free_list(bp);
}


/**
 * @brief 重新分配内存块。
 *
 * @param ptr 指向要重新分配的内存块的起始地址。如果 ptr 为 NULL，则相当于调用 mm_malloc。
 * @param size 请求的新内存块大小（以字节为单位）。该大小包括块头和块尾所需的空间。
 *
 * @return 返回重新分配的内存块的起始地址；如果重新分配失败，则返回 NULL。
 *
 * 该函数调整指定内存块的大小。如果新的大小大于原始大小，会在堆中分配新的内存块，并将原始数据复制到新内存块中。如果新的大小小于原始大小，则会截断原始数据。原始内存块的内容在重新分配之前被保留。
 */
void *mm_realloc(void *ptr, size_t size);

//测试用例
int main() {
    if (mm_init() == -1) {
        printf("Memory manager initialization failed.\n");
        return -1;
    }

    void *p1 = mm_malloc(8000);
    void *p2 = mm_malloc(200);
    void *p3 = mm_malloc(50);

    mm_free(p2);

    void *p4 = mm_malloc(150);

    mm_free(p1);
    mm_free(p3);
    mm_free(p4);

    return 0;
}