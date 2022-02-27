#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>


extern uint32_t HEAP_START, TEXT_START, RODATA_START, DATA_START,
                BSS_START, BSS_SIZE, KERNEL_STACK_START, HEAP_SIZE, TEXT_SIZE,
                RODATA_SIZE, DATA_SIZE, KERNEL_STACK_SIZE;

#define PTE_V (1 << 0)
#define PTE_R (1 << 1)
#define PTE_X (1 << 3)
#define PTE_W (1 << 2)
#define PGEOFFSET 12
#define VPNMASK 0x3ff
#define PGESIZE (1 << 12)


#define UART0 0x10000000
#define VIRTIO0 0x10001000
#define PLIC 0xc000000
#define PLICSIZE 0x2000

// Functions from pmm.c
void pmm_init(void);
uint32_t *kalloc(int n);
uint32_t *zalloc(int n);
void free(uint32_t *ptr, int n);


// Functions from paging.c
uint32_t *walk(uint32_t *pagetable, uint32_t vir_addr, int alloc);
int kmap(uint32_t *kpage, uint32_t vir_addr, uint32_t phy_addr, uint32_t size, int permission);
void kpage_init(void);
uint32_t *kpagemake(void);
void init_paging(void);

#endif
