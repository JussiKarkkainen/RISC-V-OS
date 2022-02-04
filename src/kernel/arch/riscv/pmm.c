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
void init() {
    // Bitmap starts at HEAP_START
    uint32_t *start_addr = HEAP_START;
    
    // Calculate number of bits needed for bitmap  
    int bitmap_size = HEAP_SIZE / page_size;
    
    // Clear bits in bitmap
    memset(start_addr, 0, bitmap_size);

    // Find where allocations begin aligned to a 4K boundary
    ALLOC_START = align(HEAP_START + bitmap_size, page_align);
}


// Allocates "size" amount of pages
void kalloc(int size) {
}



void free(int size) {
}



void zalloc(int size) {
}
