#include <stdint.h>
#include "../libc/include/string.h"
#include "paging.h"
#include "../libc/include/stdio.h"
#include "locks.h"

#define IS_SET(i) ((start_addr[i / 8] >> (i % 8)) & 0x1)
#define SET_BIT(i) (start_addr[i / 8] = start_addr[i / 8] | (1 << (i % 8)))
#define CLEAR_BITS(i) (start_addr[i / 8] = start_addr[i / 8] & (~(1 << (i % 8))))

#define HEAP_SIZE (MAXVA - (uint32_t)mem_end)
#define PGROUNDUP(sz)  (((sz)+PGESIZE-1) & ~(PGESIZE-1))


int page_size = (1 << 12);    // 4096
uint32_t alloc_start;
int page_align = 12;

struct spinlock pmm_lock;

void pmm_init(void) {
    initlock(&pmm_lock, "pmmlock");
    
    // Bitmap starts at end 
    uint32_t *start_addr = (uint32_t *)PGROUNDUP((uint32_t)mem_end);
    
    int bitmap_size = HEAP_SIZE / page_size;
    memset(start_addr, 0, bitmap_size);

    alloc_start = (uint32_t)PGROUNDUP((uint32_t)mem_end + bitmap_size);
}


void *kalloc(int size) {
   
    uint32_t num_pages = HEAP_SIZE / page_size;
    uint32_t *start_addr = (uint32_t *)PGROUNDUP((uint32_t)mem_end);
    int bitmap_size = HEAP_SIZE / page_size;
    alloc_start = PGROUNDUP((uint32_t)mem_end + bitmap_size);
    
    acquire_lock(&pmm_lock);

    for (uint32_t i = 0; i < num_pages; i++) {
        if (size < PGESIZE) {
            if (!IS_SET(i)) {
                SET_BIT(i);
                release_lock(&pmm_lock);
                return (uint32_t *)(alloc_start + (i * page_size));
            }
        } else {
            int num_pages = size / page_size;       // Make sure this rounds up
            if (!IS_SET(i)) {
                uint32_t j;
                int found = 1; 
                for (j = i; j < (i + num_pages); j++) {
                    if (IS_SET(j)) {
                        found = 0;
                        break;
                    }
                }
                if (found) {
                    for (int u = i; u < num_pages; u++) {
                        SET_BIT(u);
                    }
                    release_lock(&pmm_lock);
                    return (uint32_t *)(alloc_start + (i * page_size));
                }
                
            }
        }

    }
    release_lock(&pmm_lock);
    panic("no more memory, kalloc");
    return 0;
} 


uint32_t *zalloc(void) {

    uint32_t *addr = kalloc(1);

    if (addr != 0) {
        uint32_t *ptr = addr;
        int size = (page_size) / 8;
        
        for (int i = 0; i <= size; i++) {
            *ptr = 0;
            ptr++;
        }

        return addr;
    }
    return 0;
}


void kfree(void *ptr) {
    acquire_lock(&pmm_lock);
    
    uint32_t *start_addr = (uint32_t *)PGROUNDUP((uint32_t)mem_end);
    int bitmap_size = HEAP_SIZE / page_size;
    alloc_start = PGROUNDUP((uint32_t)mem_end + bitmap_size);
    
    uint32_t i = (((uint32_t)ptr - (uint32_t)alloc_start) / 0x1000);
    
    CLEAR_BITS(i);   

    release_lock(&pmm_lock);
}


// Used to verify that allocations work as expected
void test_alloc(void) { 
        
    uint32_t *a = kalloc(1);
    uint32_t *o = kalloc(1);
    uint32_t *s = kalloc(1);
    kprintf("first page %p\n", a); 
    kprintf("next page %p\n", o);
    kprintf("third page %p\n", s);
    
    int num_pages = HEAP_SIZE / page_size;
    kfree(s);
    kfree(o);
    
    uint32_t *start_addr = (uint32_t *)PGROUNDUP((uint32_t)mem_end);
    kprintf("bitmap_start %p\n", *start_addr);

    kprintf("alloc_start %p\n", alloc_start);


    uint32_t start = PGROUNDUP((uint32_t)mem_end); 
    uint32_t *p = (uint32_t *)start;
    kprintf("should be 1: %p\n", *p);
    kprintf("address of p + 1: %p\n", (p + 1));
    
    uint32_t end = start + num_pages;
    uint32_t allocation_start = alloc_start;
    uint32_t alloc_end = allocation_start + num_pages * page_size;

    kprintf("Page allocation tables\nBITMAP: %p -> %p\nPAGES: %p -> %p\n\
------------------------------------\n\n", start, end, allocation_start, alloc_end);
    
    int i = 0;
    while ((uint32_t)p < end) {
        if (*(p) == 1) {
            int beg = start;
            uint32_t memaddr = alloc_start + (beg - (uint32_t)mem_end) + PGESIZE;
            kprintf("%p -> ", memaddr);
            
            while(1) { 
                i += 1;
                if ((*(p + 1)) == 0) {
                    uint32_t end = start;
                    memaddr = alloc_start + (end - (uint32_t)mem_end) * PGESIZE + PGESIZE - 1;
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

