#include "pmm.c"
#include <stdint.h>
#include "paging.h"
#include "arch/riscv/mem.S"

extern uint32_t HEAP_START, TEXT_START, RODATA_START, DATA_START,
                BSS_START, KERNEL_STACK_START;


//typedef uint32_t *pagetable; // Pointer to a pagetable that contains 1024 PTEs
uint32_t *kpagetable;


// create the kernel pagetable
uint32_t *kpagemake(void) {

    uint32_t *kpage = zalloc(1);

    // Create a virtual memory map
    kmap(kpage, UART0, UART0, pgsize, PTE_R | PTE_W);

    kmap(kpage, VIRTIO0, VIRTIO0, pgsize, PTE_R | PTE_W);

    kmap(kpage, PLIC, PLIC, PLICSIZE, PTE_R | PTE_W);

    kmap(kpage, &HEAP_START, &HEAP_START, HEAP_SIZE, PTE_R | PTE_W);

    kmap(kpage, &TEXT_START, &TEXT_START, TEXT_SIZE, PTE_R | PTE_X);

    kmap(kpage, &RODATA_START, &RODATA_START, RODATA_SIZE, PTE_R | PTE_X);

    kmap(kpage, &DATA_START, &DATA_START, DATA_SIZE, PTE_R | PTE_W);

    kmap(kpage, &BSS_START, &BSS_START, BSS_SIZE, PTE_R | PTE_W);

    kmap(kpage, &KERNEL_STACK_START, &KERNEL_STACK_START, KERNEL_STACK_SIZE, PTE_R | PTE_W);

    return kpage;
}


// Initializes the kernel page table
void kpage_init(void) {
    kpagetable = kpagemake();
}

static inline void(void) {
    asm volatile("sfence.vma zero, zero");
}

// Write address of kernel/root pagetable to satp reg and flush TLBs
void init_paging() {
    satp_write(kpagetable);
    flush_tlb();
}

// Write kernel pagetable root address to satp and set mmu to sv32
static inline void satp_write(uint32_t kpage) {
    asm volatile("csrw %0, satp", : : "r" (1 | (kpage >> 12));
}


uint32_t *walk(uint32_t pagetable, uint32_t vir_addr, int alloc) {
    for (int i = 2; i > 0; i--) {
        uint32_t *pte = &pagetable[(vir_addr >> (PGEOFFSET + 10 * i) & VPNMASK)];
        
        // Turn pte into phy_addr
        if (*pte & PTE_V) {
            // Shift PPNs to correct places from pte
            pagetable = (uint32_t)((*pte >> 10) << 12);
        }
        // Turn phy_addr into pte
        else {
            if (!alloc || (pagetable = (uint32_t*)zalloc(1) == 0)) {
                return 0;
            }
            *pte = ((pagetable >> 12) << 10) | PTE_V;
        }
    }
    return &pagetable[((vir_addr >> PGEOFFSET) & VPNMASK)];
}   

 
int kmap(uint32_t *kpage, uint32_t vir_addr, uint32_t phy_addr, uint32_t size, int permissions) {

    uint32_t *pte;
    uint32_t last;
    uint32_t vir;

    if (size > 0) {
        last = vir_addr + size - 1;
        vir = vir_addr;

        while(1) {
            pte = walk(kpage, vir_addr, 1);
            if (pte == 0) {
                return -1;
            }
            if (*pte & PTE_V == 0) {
                *pte = (phy_addr >> 12) << 10;

                if (vir = last) {
                    break;
                }
                a += PGESIZE;
                phy_addr += PGESIZE;
            }
        
        return 0;
        }
}

