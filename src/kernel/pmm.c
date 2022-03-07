#include <stdint.h>
#include "../libc/include/string.h"
#include <stdbool.h>
#include "paging.h"
#include "../libc/include/stdio.h"

/* 
Implementation of a physical memory allocator. It uses a bitmap
to keep track of size 4096 pages
*/

extern uint32_t HEAP_START, TEXT_START, RODATA_START, DATA_START, 
                BSS_START, KERNEL_STACK_START, HEAP_SIZE;

// Aligns memory to 4K page
int align(int value, int align) {
    int c = (1 << align) - 1;
    return (value + (~value & c));
}

int page_size = (1 << 12);    // 4096 kib
uint32_t alloc_start;
int page_align = 12;

// Initializes the bitmap by clearing required area
void pmm_init(void) {

    // Bitmap starts at HEAP_START
    uint32_t *start_addr = (uint32_t *)HEAP_START;
    
    // Calculate number of bits needed for bitmap  
    int bitmap_size = HEAP_SIZE / page_size;
    
    // Clear bits in bitmap
    memset(start_addr, 0, bitmap_size);

    // Find where allocations begin aligned to a 4K boundary
    alloc_start = align(HEAP_START + bitmap_size, page_align);
}


// Allocates n amount of pages, by setting bits in bitmap to 1, 
// and returning pointer to start of allocated memory
uint32_t *kalloc(int n) {
   
    int num_pages = HEAP_SIZE / page_size;
    uint32_t *start_addr = (uint32_t *)HEAP_START;
    int bitmap_size = HEAP_SIZE / page_size;
    alloc_start = align(HEAP_START + bitmap_size, page_align);
    
    // Search for contiguos blockof free memory of size "size"
    uint32_t *ptr = start_addr;
    
    for (int i = 0; i < num_pages; i++) {
        int found = false;

        if (*(ptr + i) == 0) {
            found = true;
            for (int j = 0; j <= (i + n); j++) {

                if (*(ptr + j) == 1) {
                    found = false;
                }
            }
        }

        // If found is true, we have a contiguos area to allocate
        if (found) {
            for (int k = 0; k <= num_pages; k++) {
                *ptr = 1;
            }
            uint32_t *ret_addr = (uint32_t *)(alloc_start + page_size * i);
            return ret_addr;
        }
    }
    return 0;
}

// Zero allocates n amount of pages
uint32_t *zalloc(int n) {

    uint32_t *addr = kalloc(n);

    if (addr != 0) {
        uint32_t *ptr = addr;
        int size = (page_size * n) / 8;
        
        for (int i = 0; i <= size; i++) {
            *ptr = 0;
            ptr++;
        }

        return addr;
    }
    return 0;
}

// Free size n amount of pages
void free(uint32_t *ptr, int n) {
    if (ptr != 0) {
        
        // Calculate where the corresponding bit is bitmap is
        uint32_t *addr = (uint32_t *)(HEAP_START + (int)(ptr - alloc_start) / page_size);

        if (addr >= (uint32_t *)HEAP_START && addr < (uint32_t *)(HEAP_SIZE + HEAP_SIZE)) {
            for (int i = 0; i <= n; i++) {
                *addr = 0; 
                addr++;
                
            } 
        }
    }
}

/*
void test_alloc(void) {
    // Used to verify that allocations work as expected
    
    int num_pages = &HEAP_SIZE / page_size;
    int start = &HEAP_START;
    int end = start + num_pages;
    int allocation_start = alloc_start;
    int alloc_end = allocation_start + num_pages * page_size;

    kprintf("Page allocation tables:\n Bitmap: 0x%x\n PhyAlloc: 0x%x\n
            ------------------------------------\n", start, end, allocation_start, alloc_end);


}
*/
