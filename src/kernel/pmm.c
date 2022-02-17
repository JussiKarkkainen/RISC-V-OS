#include <stdint.h>
#include "../libc/include/string.h"
#include "arch/riscv/mem.S"
#include <stdbool.h>
#include "paging.h"

/* 
Implementation of a physical memory allocator. It uses a bitmap
to keep track of size 4096 pages
*/


// Aligns memory to 4K page
int align(int value, int align) {
    int c = (1 << align) - 1;
    return (value + (~value & c));
}

int page_size = (1 << 12);    // 4096 kib
uint32_t alloc_start;

// Initializes the bitmap by clearing required area
void pmm_init(void) {


    int page_align = 12;
    
    // Bitmap starts at HEAP_START
    uint32_t *start_addr = &HEAP_START;
    
    // Calculate number of bits needed for bitmap  
    int bitmap_size = &HEAP_SIZE / page_size;
    
    // Clear bits in bitmap
    memset(start_addr, 0, bitmap_size);

    // Find where allocations begin aligned to a 4K boundary
    alloc_start = align(&HEAP_START + bitmap_size, page_align);
}


// Allocates n amount of pages, by setting bits in bitmap to 1, 
// and returning pointer to start of allocated memory
uint32_t *kalloc(size_t n) {
   
    int num_pages = &HEAP_SIZE / page_size;
    uint32_t *start_addr = &HEAP_START;
    int bitmap_size = &HEAP_SIZE / page_size;
    
    // Search for contiguos blockof free memory of size "size"
    uint32_t *ptr;
    
    for (int i = 0; i < num_pages; i++) {
        int found = false;

        if (ptr + i == 0) {
            found = true;

            for (int j = 0; j <= (i + n); j++) {

                if (ptr + j == 1) {
                    found = false;
                    break;
                }
            }
        }

        // If found is true, we have a contiguos area to allocate
        if (found) {
            for (int k = 0; k <= num_pages; k++) {
                *ptr = 1;
            }
            uint32_t *ret_addr = alloc_start + page_size * i;
            
            return  ret_addr;
        }
    }
    return 0;
}

// Zero allocates n amount of pages
uint32_t *zalloc(size_t n) {

    uint32_t *addr = kalloc(n);

    if (addr != NULL) {
        uint32_t *ptr = addr;
        int size = (page_size * n) / 8;
        
        for (int i = 0; i <= size; i++) {
            *ptr = 0;
            ptr++;
        
    return addr;

        }
    }
}

// Free size n amount of pages
void free(uint32_t *ptr, size_t n) {
    if (ptr != NULL) {
        
        // Calculate where the corresponding bit is bitmap is
        uint32_t *addr = &HEAP_START + (ptr - alloc_start) / page_size;

        if (addr >= &HEAP_START && addr < (&HEAP_SIZE + &HEAP_SIZE)) {
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
