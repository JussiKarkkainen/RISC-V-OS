#include <stdint.h>
#include "../../libc/include/string.h"
#include <mem.S>

/* 
Implementation of a physical memory allocator. It uses a bitmap
to keep track of size 4096 pages
*/


uint32_t ALLOC_START
int page_size = 1 << 12;    // 4096 kib
int page_align = 12;

// Aligns memory to 4K page
int align(int value, int align) {
    int c = (1 << align) - 1;
    return value + (!value & c);
}


// Initializes the bitmap by clearing required area
void pmm_init() {
    // Bitmap starts at HEAP_START
    uint32_t *start_addr = HEAP_START;
    
    // Calculate number of bits needed for bitmap  
    int bitmap_size = HEAP_SIZE / page_size;
    
    // Clear bits in bitmap
    memset(start_addr, 0, bitmap_size);

    // Find where allocations begin aligned to a 4K boundary
    ALLOC_START = align(HEAP_START + bitmap_size, page_align);
}


// Allocates n amount of pages, by setting bits in bitmap to 1, 
// and returning pointer to start of allocated memory
uint32_t *kalloc(size_t n) {
   
    uint32_t *start_addr = HEAP_START;
    int bitmap_size = HEAP_SIZE / page_size;
    
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
            for (int k = 0; k <= pages; k++) {
                *ptr = 1;
            
            uint32_t *ret_addr = ALLOC_START + page_size * i;
            
            return  ret_addr;
    }
}

// Zero allocates n amount of pages
uint32_t *zalloc(size_t n) {

    uint32_t *addr = kalloc(n);

    if (ret != NULL) {
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
        uint32_t *addr = HEAP_START + (ptr - ALLOC_START) / page_size;

        if (addr >= HEAP_START && addr < (HEAP_SIZE + HEAP_SIZE)) {
            for (int i = 0; i <= n; i++) {
                *addr = 0; 
                addr++;
                
            } 
        }
    }
}


void test_alloc() {
    // Used to verify that allocations work as expected
    
    int num_pages = HEAP_SIZE / page_size;
    int start = HEAP_START;
    int end = start + num_pages;
    int alloc_start = ALLOC_START;
    int alloc_end = ALLOC_START + num_pages * page_size;

    kprintf("Page allocation tables:\n Bitmap: 0x%x\n PhyAlloc: 0x%x\n
            ------------------------------------\n", start, end, alloc_start, alloc_end);


}