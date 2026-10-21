#include "HARD-2.1.h"
/**
 * @brief 初始化内存分配器
 *
 * @return 成功时返回 0，失败时返回 -1
 */
int mm_init(void)
{
    // 初始化堆
    if ((heap_listp = sbrk(4*WSIZE)) == (void*)-1)
        return -1;

    // 初始化空闲链表
    PUT(heap_listp, 0);
    PUT(heap_listp + 1*WSIZE, PACK(8, 1));
    PUT(heap_listp + 2*WSIZE, PACK(8, 1));
    PUT(heap_listp, PACK(0, 1));
    heap_listp += (2*WSIZE);

    // 扩展堆
    if (extend_heap(CHUNKSIZE) == NULL)
        return -1;

    return 0;
}
/**
 * @brief 打印当前堆块的状态信息。
 *
 * 根据传入的 verbose 参数决定是否打印堆块信息。此函数用于调试，
 * 以检查堆内存布局、块大小及其分配状态。
 *
 * @param verbose  控制是否打印输出。当 verbose 为 1 时打印信息，为 0 时不打印。
 * @param func     表示调用此函数的上下文或函数名称，用于输出中标识当前的操作阶段。
 */
static void print_heap_blocks(int verbose, const char* func) {
    if (!verbose) return;  // 如果不需要输出调试信息，则直接返回

    char *curbp = (char *)heap_listp;

    printf("\n=========================== %s ===========================\n", func);

    while (GET_SIZE(HDRP(curbp)) > 0) {
        printf("Address: %p\n", curbp);
        printf("Size: %d\n", GET_SIZE(HDRP(curbp)));
        printf("Allocated: %d\n", GET_ALLOC(HDRP(curbp)));
        printf("\n");
        curbp = NEXT_BLKP(curbp);
    }

    printf("Address: %p\n", curbp); // Should point to the end of the heap
    printf("Size: 0\n");
    printf("Allocated: 1\n");
    printf("=========================== %s ===========================\n", func);
}


/**
 * @brief 扩展堆的大小以适应请求的内存。
 *
 * 这个函数通过调用 sbrk 来扩展堆的大小。扩展的大小会被调整为CHUNKSIZE的整数倍，以便更好地管理堆内存。
 *
 * @param size 需要扩展的内存大小（以字节为单位）。
 *
 * @note 初始调用mm_init时扩展一个CHUNKSIZE
 * @return 扩展后的内存块的指针，如果扩展失败则返回 NULL。
 */
static void *extend_heap(size_t size) {
    void *p=sbrk(size);
    if(p==(void*)-1){return NULL;}
    PUT(HDRP(p),PACK(size,0));
    PUT(FTRP(p),PACK(size,0));
    PUT(HDRP(NEXT_BLKP(p)),PACK(0,1));
    return p;
}


/**
 * @brief 在堆中查找合适的空闲块。
 *
 * 这个函数遍历堆中的所有块，寻找一个大小足够且未分配的空闲块。
 *
 * @param size 需要分配的内存块的大小（以字节为单位）。
 *
 * @return 找到的合适的空闲块的指针，如果没有找到则返回 `NULL`。
 */
static void *find_fit(size_t size) {
    char* find=heap_listp;
    while(GET_SIZE(HDRP(find))>0){
        if(!GET_ALLOC(HDRP(find))&&GET_SIZE(HDRP(find))>=size){
            return find;
        }
    find=NEXT_BLKP(find);
    }
    return NULL;
}


/**
 * @brief 将内存块分配给用户。
 *
 * 这个函数会根据请求的大小 asize(对齐后的size) 将一个内存块分配给用户。如果分配的块剩余的空间足够大，则将剩余的空间保留为一个新的空闲块。否则，将整个块都分配出去。
 *
 * @param bp 指向要分配的内存块的指针。
 * @param asize 需要分配的内存块的大小（以字节为单位）。
 */
static void place(char *bp, size_t asize) {
    size_t size = GET_SIZE(HDRP(bp));

    if (size - asize < MINBLOCK) {
        PUT(HDRP(bp), PACK(size, 1));
        PUT(FTRP(bp), PACK(size, 1));
    } else {
        PUT(HDRP(bp), PACK(asize, 1));
        PUT(FTRP(bp), PACK(asize, 1));
        void *next = NEXT_BLKP(bp);
        PUT(HDRP(next), PACK(size - asize, 0));
        PUT(FTRP(next), PACK(size - asize, 0));
    }
}

/**
 * @brief 分配一个指定大小的内存块。
 *
 * 这个函数会根据请求的内存大小size来分配内存。如果请求的大小为零，则返回 NULL。首先，函数尝试从空闲块列表中找到一个合适的块，如果找到，则进行分配。如果没有找到合适的块，则会扩展堆的大小，并再次尝试分配内存。
 *
 * @param size 需要分配的内存大小（以字节为单位）。
 *
 * @return 指向分配的内存块的指针，如果分配失败则返回 `NULL`。
 */
void *mm_malloc(size_t size) {
    if(size==0){return NULL;}
    size_t asize;
    if(size<=DSIZE){
        asize=MINBLOCK;
    }else{
        asize=ALIGN(size+DSIZE);
    }
    void* bp=find_fit(asize);
    if(bp!=NULL){
        place(bp,asize);
        print_heap_blocks(1,"mm_malloc");
        return bp;
    }
    size_t extend_size=MAX(asize,CHUNKSIZE);
    bp=extend_heap(extend_size);
    if(bp==NULL){
        return bp;
    }
    place(bp,asize);
    print_heap_blocks(1,"mm_malloc");
    return bp;
}


/**
 * @brief 合并相邻的空闲块。
 *
 * 这个函数检查指定块 bp 的前后相邻块的分配状态。如果相邻的块为空闲块，则将这些块合并成一个更大的空闲块，并更新其头部和尾部。函数会返回合并后的空闲块的指针。
 *
 * @param bp 指向要进行合并操作的内存块的指针。
 *
 * @return 合并后的空闲块的指针。
 */
static void *coalesce(void *bp) {
    int next_alloc=GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    int prev_alloc=GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t size=GET_SIZE(HDRP(bp));
    if(!next_alloc&&!prev_alloc){
        size=
        size+
        GET_SIZE(HDRP(NEXT_BLKP(bp)))+
        GET_SIZE((HDRP(PREV_BLKP(bp))));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
        bp=PREV_BLKP(bp);
    }
    else if(next_alloc&&!prev_alloc){
        size=size+GET_SIZE(HDRP(PREV_BLKP(bp)));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        PUT((FTRP(bp)),PACK(size,0));
        bp=PREV_BLKP(bp);
    }
    else if(!next_alloc&&prev_alloc){
        size=size+GET_SIZE(HDRP(NEXT_BLKP(bp)));
        PUT(HDRP(bp),PACK(size,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
    }
    return bp;
}


/**
 * @brief 释放一个已分配的内存块。
 *
 * 这个函数将指定的内存块 ptr 标记为未分配，并尝试将其与相邻的空闲块合并。
 *
 * @param ptr 指向要释放的内存块的指针。
 */
void mm_free(void *ptr) {
    if(ptr==NULL)return;
    size_t size=GET_SIZE(HDRP(ptr));
    PUT(HDRP(ptr),PACK(size,0));
    PUT(FTRP(ptr),PACK(size,0));
    coalesce(ptr);
    print_heap_blocks(1,"mm_free");
}


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