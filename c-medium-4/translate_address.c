#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#define PAGE_DIR_BITS   10 // 页目录索引位数
#define PAGE_TABLE_BITS 10 // 页表索引位数
#define OFFSET_BITS     12 // 页内偏移位数

#define PAGE_TABLE_SIZE (1 << PAGE_TABLE_BITS) // 1024
#define PAGE_DIR_SIZE   (1 << PAGE_DIR_BITS)   // 1024
#define PAGE_SIZE       (1 << OFFSET_BITS)     // 4096 (4KB)

// 页表项 (PTE - Page Table Entry)
// 包含物理帧号和权限位
typedef struct {
    bool     present_bit; // 存在位：1表示该页在物理内存中，0表示不在
    bool     rw_bit;      // 读写位：1表示可读可写，0表示只读
    uint32_t frame_number; // 对应的物理帧号
} PTE;

// 页目录项 (PDE - Page Directory Entry)
// 包含指向页表的指针和存在位
typedef struct {
    bool present_bit; // 存在位：1表示该页目录项指向一个有效的页表，0表示无效
    PTE* page_table_base; // 指向一个页表（PTE数组）的指针
} PDE;

// 模拟的页目录表（作为我们模拟的 "CR3寄存器" 指向的地址）
PDE page_directory[PAGE_DIR_SIZE];


/**
 * @brief 模拟MMU进行地址翻译和权限检查
 * * @param virtual_address 要翻译的32位虚拟地址
 * @param is_write_access 访问类型，true表示写操作，false表示读操作
 */
void translate_address(uint32_t virtual_address, bool is_write_access) {//请你完成这个尚未完成的函数
    printf("----------------------------------------\n");
    printf("Translating Virtual Address: 0x%08X (%s access)\n", virtual_address, is_write_access ? "WRITE" : "READ");

    // --- 步骤1: 从虚拟地址中提取索引和偏移 ---
    uint32_t page_dir_index;
    uint32_t page_table_index;
    uint32_t offset;
    // 利用位运算（移位 >> 和 掩码 &）提取对应部分
    // 请补全代码写在TO DO与END OF TO DO之间：
    // TO DO:
    page_dir_index=(virtual_address>>22)&0x3ff;
    page_table_index=(virtual_address>>12)&0x3ff;
    offset=virtual_address&0xfff;
    // END OF TO DO：

    printf(" -> Page Dir Index: %u (0x%X)\n", page_dir_index, page_dir_index);
    printf(" -> Page Table Index: %u (0x%X)\n", page_table_index, page_table_index);
    printf(" -> Offset: %u (0x%X)\n", offset, offset);

    // --- 步骤2: 查询页目录表 ---
    // 请补全代码写在TO DO与END OF TO DO之间：
    // TO DO:
    printf(" [*] Checking Page Directory Entry %u...\n",page_dir_index);
    if(!page_directory[page_dir_index].present_bit){
        printf(" [!] FAULT: Page Directory Entry not present. (Segmentation Fault)\n");
        return;
    }
    printf("     -> PDE is present. Page table base address: %p\n",page_directory[page_dir_index].page_table_base);
    // END OF TO DO：

    // --- 步骤3: 查询页表 ---
    // 请补全代码写在TO DO与END OF TO DO之间：
    // TO DO:
    printf(" [*] Checking Page Table Entry %u...\n",page_table_index);
    if(!page_directory[page_dir_index].page_table_base[page_table_index].present_bit){
        printf(" [!] FAULT: Page Table Entry not present. (Page Fault)\n");
        return;
    }
    printf("     -> PTE is present. Frame number: %u (0x%x)\n",page_directory[page_dir_index].page_table_base[page_table_index].frame_number,page_directory[page_dir_index].page_table_base[page_table_index].frame_number);
    // END OF TO DO：
    // --- 步骤4: 检查访问权限 ---
    // 请补全代码写在TO DO与END OF TO DO之间：
    // TO DO:
    printf(" [*] Checking access permissions...\n");
    if(is_write_access&&!page_directory[page_dir_index].page_table_base[page_table_index].rw_bit){
        printf(" [!] FAULT: Write attempt on a read-only page. (Protection Fault)\n");
        return;
    }
    printf("     -> Access granted.\n");
    // END OF TO DO：
    // --- 步骤5: 计算最终的物理地址 ---
    // 请补全代码写在TO DO与END OF TO DO之间：
    // TO DO:
    uint32_t physical_address;
     physical_address =
    (page_directory[page_dir_index]
         .page_table_base[page_table_index].frame_number
     << OFFSET_BITS)| offset;
     uint32_t frame_number =
    page_directory[page_dir_index]
        .page_table_base[page_table_index].frame_number;
    // END OF TO DO：
    printf("  [SUCCESS] Translation complete.\n");
    printf("  Virtual Address 0x%08X  =>  Physical Address 0x%08X\n", virtual_address, physical_address);
}

/**
 * @brief 初始化模拟环境，预设一些页表和页目录项
 */
void initialize_simulation() {
    printf("Initializing MMU simulation environment...\n");

    // 初始化整个页目录表
    for (int i = 0; i < PAGE_DIR_SIZE; ++i) {
        page_directory[i].present_bit = false;
        page_directory[i].page_table_base = NULL;
    }

    // 2. 创建并填充第一个页表 (用于虚拟地址 0x00000000 - 0x003FFFFF)
    //    假设页目录索引为0
    PTE* page_table_1 = (PTE*)malloc(sizeof(PTE) * PAGE_TABLE_SIZE);

    page_directory[0].present_bit = true;
    page_directory[0].page_table_base = page_table_1;

    for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
        page_table_1[i].present_bit = false; // 默认所有PTE无效
    }
    // 设置几个有效的PTE
    // VA 0x00001xxx -> PA 0x0001Axxx (可读可写)
    page_table_1[1].present_bit = true;
    page_table_1[1].rw_bit = true;
    page_table_1[1].frame_number = 26; // 物理帧号 0x1A

    // VA 0x00002xxx -> PA 0x0008Fxxx (只读)
    page_table_1[2].present_bit = true;
    page_table_1[2].rw_bit = false; // 只读页面
    page_table_1[2].frame_number = 143; // 物理帧号 0x8F

    // 3. 创建并填充第二个页表 (用于虚拟地址 0x00400000 - 0x007FFFFF)
    //    假设页目录索引为1
    PTE* page_table_2 = (PTE*)malloc(sizeof(PTE) * PAGE_TABLE_SIZE);
    page_directory[1].present_bit = true;
    page_directory[1].page_table_base = page_table_2;

    for (int i = 0; i < PAGE_TABLE_SIZE; ++i) {
        page_table_2[i].present_bit = false;
    }
    // VA 0x00400xxx -> PA 0x00033xxx
    page_table_2[0].present_bit = true;
    page_table_2[0].rw_bit = true;
    page_table_2[0].frame_number = 51; // 物理帧号 0x33

    printf("Initialization complete.\n\n");
}

// --- 4. 主函数，运行测试用例 ---

int main() {
    initialize_simulation();

    // --- 测试用例 ---

    // 1. 成功读取: 访问一个有效的、可读写的地址
    // 虚拟地址: 0x00001A2B
    // -> 页目录索引: 0, 页表索引: 1, 偏移: 0xA2B
    // -> 查找 PDE[0] -> PTE[1] -> 物理帧号 26 (0x1A)
    // -> 物理地址: (26 << 12) | 0xA2B = 0x1A000 | 0xA2B = 0x1AA2B
    translate_address(0x00001A2B, false);

    // 2. 成功写入: 访问一个有效的、可读写的地址
    // 与上面相同，但请求是写操作
    translate_address(0x00001A2B, true);

    // 3. 保护错误: 尝试写入一个只读页面
    // 虚拟地址: 0x00002048
    // -> 页目录索引: 0, 页表索引: 2, 偏移: 0x048
    // -> 查找 PDE[0] -> PTE[2] -> rw_bit = 0, 触发保护错误
    translate_address(0x00002048, true);

    // 4. 缺页错误: 访问一个页表项(PTE)无效的地址
    // 虚拟地址: 0x00003555
    // -> 页目录索引: 0, 页表索引: 3, 偏移: 0x555
    // -> 查找 PDE[0] -> PTE[3] -> present_bit = 0, 触发缺页错误
    translate_address(0x00003555, false);

    // 5. 段错误: 访问一个页目录项(PDE)无效的地址
    // 虚拟地址: 0x00804000 (页目录索引=2)
    // -> 页目录索引: 2
    // -> 查找 PDE[2] -> present_bit = 0, 触发段错误
    translate_address(0x00804000, false);

    // --- 释放动态分配的内存 ---
    // 在真实OS中，这部分内存管理会更复杂
    free(page_directory[0].page_table_base);
    free(page_directory[1].page_table_base);

    return 0;
}



/*
sample output:

Initializing MMU simulation environment...
Initialization complete.

----------------------------------------
Translating Virtual Address: 0x00001A2B (READ access)
 -> Page Dir Index: 0 (0x0)
 -> Page Table Index: 1 (0x1)
 -> Offset: 2603 (0xA2B)
  [*] Checking Page Directory Entry 0...
      -> PDE is present. Page table base address: 0x60000305c010
  [*] Checking Page Table Entry 1...
      -> PTE is present. Frame number: 26 (0x1A)
  [*] Checking access permissions...
      -> Access granted.
  [SUCCESS] Translation complete.
  Virtual Address 0x00001A2B  =>  Physical Address 0x0001AA2B
----------------------------------------
Translating Virtual Address: 0x00001A2B (WRITE access)
 -> Page Dir Index: 0 (0x0)
 -> Page Table Index: 1 (0x1)
 -> Offset: 2603 (0xA2B)
  [*] Checking Page Directory Entry 0...
      -> PDE is present. Page table base address: 0x60000305c010
  [*] Checking Page Table Entry 1...
      -> PTE is present. Frame number: 26 (0x1A)
  [*] Checking access permissions...
      -> Access granted.
  [SUCCESS] Translation complete.
  Virtual Address 0x00001A2B  =>  Physical Address 0x0001AA2B
----------------------------------------
Translating Virtual Address: 0x00002048 (WRITE access)
 -> Page Dir Index: 0 (0x0)
 -> Page Table Index: 2 (0x2)
 -> Offset: 72 (0x48)
  [*] Checking Page Directory Entry 0...
      -> PDE is present. Page table base address: 0x60000305c010
  [*] Checking Page Table Entry 2...
      -> PTE is present. Frame number: 143 (0x8F)
  [*] Checking access permissions...
  [!] FAULT: Write attempt on a read-only page. (Protection Fault)
----------------------------------------
Translating Virtual Address: 0x00003555 (READ access)
 -> Page Dir Index: 0 (0x0)
 -> Page Table Index: 3 (0x3)
 -> Offset: 1365 (0x555)
  [*] Checking Page Directory Entry 0...
      -> PDE is present. Page table base address: 0x60000305c010
  [*] Checking Page Table Entry 3...
  [!] FAULT: Page Table Entry not present. (Page Fault)
----------------------------------------
Translating Virtual Address: 0x00804000 (READ access)
 -> Page Dir Index: 2 (0x2)
 -> Page Table Index: 4 (0x4)
 -> Offset: 0 (0x0)
  [*] Checking Page Directory Entry 2...
  [!] FAULT: Page Directory Entry not present. (Segmentation Fault)
*/