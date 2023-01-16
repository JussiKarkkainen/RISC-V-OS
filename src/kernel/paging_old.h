#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

/*
extern uint32_t MEMORY_END, MEMORY_END, HEAP_START, TEXT_START, RODATA_START, DATA_START,
                BSS_START, BSS_SIZE, KERNEL_STACK_START, HEAP_SIZE, TEXT_SIZE,
                RODATA_SIZE, DATA_SIZE, KERNEL_STACK_SIZE;
*/

extern char mem_end[];

#define PTE_V (1L << 0)
#define PTE_R (1L << 1)
#define PTE_X (1L << 3)
#define PTE_W (1L << 2)
#define PTE_U (1L << 4)
#define PGEOFFSET 12
#define VPNMASK 0x3ff
#define PGESIZE (1 << 12)
#define KERNEL_BASE 0x80000000
#define MAXVA 0xffffffff
#define PHYSTOP 0x88000000
#define USERVEC (MAXVA - PGESIZE + 1)
#define UART0 0x10000000
#define VIRTIO0 0x10001000
#define CLINT 0x02000000
#define CLINT_OFFSET 0x4000
#define PLIC 0x0C000000
#define PLICSIZE 0x400000
#define TRAPFRAME (USERVEC - PGESIZE)

// Functions from pmm.c
void pmm_init(void);
void *kalloc(void);
//uint32_t *zalloc(void);
void kfree(void *ptr);
void test_alloc(void);

// Functions from paging.c
uint32_t *walk(uint32_t *pagetable, uint32_t vir_addr, int alloc);
uint32_t fetch_pa_addr(uint32_t *pagetable, uint32_t va);
int kmap(uint32_t *kpage, uint32_t vir_addr, uint32_t phy_addr, uint32_t size, int permission);
void kpage_init(void);
uint32_t *kpagemake(void);
void init_paging(void);
void test_alloc(void);
void upaging_init(uint32_t *pagetable, unsigned char *src, unsigned int size);
int copyto(uint32_t *pagetable, char *dst, uint32_t srcaddr, uint32_t len);
int copyout(uint32_t *pagetable, uint32_t dstaddr, char *src, uint32_t len);
int copyinstr(uint32_t *pagetable, char *dst, uint32_t srcva, uint32_t max);
uint32_t *upaging_create(void);
uint32_t uvmalloc(uint32_t *pagetable, uint32_t oldsize, uint32_t newsize);
uint32_t uvmdealloc(uint32_t *pagetable, uint32_t oldsize, uint32_t newsize);
void uvmclear(uint32_t *pagetable, uint32_t va);
void uvmunmap(uint32_t *pagetable, uint32_t va, uint32_t num_pages, int free);
void uvmfree(uint32_t *pagetable, uint32_t size);
void freewalk(uint32_t *pagetable);
int uvmcopy(uint32_t *old, uint32_t *new_addr, uint32_t size);

#endif
