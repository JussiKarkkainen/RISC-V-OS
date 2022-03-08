#include <stdint.h>
#include "../libc/include/string.h"
#include <stdbool.h>
#include "paging.h"
#include "../libc/include/stdio.h"

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
int page_align = 12;

// Initializes the bitmap by clearing required area
void pmm_init(void) {
    kprintf("HEAP_SIZE %p\n", HEAP_SIZE);
    // Bitmap starts at HEAP_START
    uint32_t *start_addr = (uint32_t *)HEAP_START;
    kprintf("start_addr %p\n", start_addr);
    
    // Calculate number of bits needed for bitmap  
    int bitmap_size = HEAP_SIZE / page_size;
    kprintf("bitmap_size %x\n", bitmap_size);
    // Clear bits in bitmap
    memset(start_addr, 0, bitmap_size);

    // Find where allocations begin aligned to a 4K boundary
    alloc_start = align(HEAP_START + bitmap_size, page_align);
    kprintf("alloc_start %p\n", alloc_start);
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
            for (int j = i; j <= (i + n); j++) {

                if (*(ptr + j) == 1) {
                    found = false;
                    break;
                }
            }
        }

        // If found is true, we have a contiguos area to allocate
        if (found) {
            for (int k = i; k <= (i + n - 1); k++) {
                *(ptr + k) = 1;
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


void test_alloc(void) {
    // Used to verify that allocations work as expected
    uint32_t *a = kalloc(10);
    uint32_t *o = zalloc(5);
    kprintf("first ten pages %p\n", a); 
    kprintf("next five pagse %p\n", o);
    int num_pages = HEAP_SIZE / page_size;
    uint32_t start = HEAP_START;
    uint32_t *p = (uint32_t *)start;
    uint32_t d = *p;
    kprintf("should be 1: %p\n", d);
    kprintf("address of p + 1: %p\n", (p + 1));
    kprintf("this should also be 1: %p\n\n\n", *(p + 15));
    uint32_t end = start + num_pages;
    uint32_t allocation_start = alloc_start;
    uint32_t alloc_end = allocation_start + num_pages * page_size;

    kprintf("Page allocation tables\nBITMAP: %p -> %p\nPAGES: %p -> %p\n\
------------------------------------\n\n", start, end, allocation_start, alloc_end);
    
    int i = 0;
    while (p < end) {
        if (*(p) == 1) {
            int beg = start;
            uint32_t memaddr = alloc_start + (beg - HEAP_START) + PGESIZE;
            kprintf("%p -> ", memaddr);
            
            while(1) { 
                i += 1;
                if ((*(p + 1)) == 0) {
                    uint32_t end = start;
                    memaddr = alloc_start + (end - HEAP_START) * PGESIZE + PGESIZE - 1;
                    kprintf("%p : %x pages\n", memaddr, (end - beg + 1));
                    break;
                }
                p = p + 1;
            }
        }
        p = p + 1;
    }

    kprintf("------------------------------------\n\n");

    kprintf("Allocated %x pages and %x bytes\n", i, (i * PGESIZE));
    kprintf("Free %x pages and %x bytes\n\n\n", (num_pages - 1), ((num_pages - i) * PGESIZE));
}

