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


void *kalloc(void) {
   
    uint32_t *start_addr = (uint32_t *)PGROUNDUP((uint32_t)mem_end);
    uint32_t num_pages = (PHYSTOP - (uint32_t)start_addr) / PGESIZE;
    alloc_start = PGROUNDUP((uint32_t)mem_end + num_pages);
    acquire_lock(&pmm_lock);

    for (uint32_t i = 0; i < num_pages; i++) {
        if (!IS_SET(i)) {
            SET_BIT(i);
            release_lock(&pmm_lock);
            uint32_t ret = (alloc_start + (i * page_size));
            return (void *)ret;
        }
    }
    release_lock(&pmm_lock);
    panic("no more memory, kalloc");
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


