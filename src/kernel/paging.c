#include <stdint.h>
#include "paging.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include "process.h"

uint32_t *kpagetable;

static inline void flush_tlb(void) {
    asm volatile("sfence.vma zero, zero");
}

// Write kernel pagetable root address to satp and set mmu to sv32
static inline void satp_write(uint32_t *kpage) {
    asm volatile("csrw satp, %0" : : "r" (1 | ((uint32_t)kpage >> 12)));

}

// create the kernel pagetable
uint32_t *kpagemake(void) {

    uint32_t *kpage = zalloc(1);

    // Create a virtual memory map
    kmap(kpage, UART0, UART0, PGESIZE, PTE_R | PTE_W);

    kmap(kpage, VIRTIO0, VIRTIO0, PGESIZE, PTE_R | PTE_W);

    kmap(kpage, PLIC, PLIC, PLICSIZE, PTE_R | PTE_W);

    kmap(kpage, HEAP_START, HEAP_START, HEAP_SIZE, PTE_R | PTE_W);

    kmap(kpage, TEXT_START, TEXT_START, TEXT_SIZE, PTE_R | PTE_X);

    kmap(kpage, RODATA_START, RODATA_START, RODATA_SIZE, PTE_R | PTE_X);

    kmap(kpage, DATA_START, DATA_START, DATA_SIZE, PTE_R | PTE_W);

    kmap(kpage, BSS_START, BSS_START, BSS_SIZE, PTE_R | PTE_W);         // BSS_SIZE is apparently zero, needs fixing

    kmap(kpage, KERNEL_STACK_START, KERNEL_STACK_START, KERNEL_STACK_SIZE, PTE_R | PTE_W);

    map_kstack(kpage);

    return kpage;
}

void map_kstack(uint32_t *pagetable) {
    struct process *proc;

    for (proc = process; proc < &process[MAXPROC]; proc++) {
        uint32_t *phy_addr = kalloc(1);
        if (phy_addr == 0) {
            kprintf("phy_addr %p", phy_addr);
        }
        uint32_t va = (USERVEC - ((proc - process) + 1) * 2 * PGESIZE);
        kmap(pagetable, va, (uint32_t)phy_addr, PGESIZE, PTE_W | PTE_R);
    }
}

// Initializes the kernel page table
void kpage_init(void) {
    kpagetable = kpagemake();
}

// Write address of kernel/root pagetable to satp reg and flush TLBs
void init_paging(void) {
    satp_write(kpagetable);
    flush_tlb();
}

uint32_t *walk(uint32_t *pagetable, uint32_t vir_addr, int alloc) {
    for (int i = 2; i > 0; i--) {
        uint32_t *pte = &pagetable[(vir_addr >> (PGEOFFSET + 10 * i) & VPNMASK)];
        
        // Turn pte into phy_addr
        if (*pte & PTE_V) {
            // Shift PPNs to correct places from pte
            pagetable = (uint32_t *)((*pte >> 10) << 12);
        }
        // Turn phy_addr into pte
        else {
            if (!alloc || ((pagetable = zalloc(1)) == 0)) {
                return 0;
            }
            *pte = (((uint32_t)pagetable >> 12) << 10) | PTE_V;
        }
    }
    return &pagetable[((vir_addr >> PGEOFFSET) & VPNMASK)];
}   

 
int kmap(uint32_t *kpage, uint32_t vir_addr, uint32_t phy_addr, uint32_t size, int permissions) {

    uint32_t *pte;
    uint32_t last;
    uint32_t vir;

    if (size == 0) {
        panic("kmap: size == 0!");
    }
    last = vir_addr + size - 1;
    vir = vir_addr;

    while(1) {
        pte = walk(kpage, vir_addr, 1);
        if (pte == 0) {
            return -1;
        }
        if (*pte & PTE_V) {
            panic("kmap()");
        }
        *pte = ((phy_addr >> 12) << 10) | permissions | PTE_V;

        if (vir == last) {
            break;
        }
        vir += PGESIZE;
        phy_addr += PGESIZE;
    }
    return 0;
}

uint32_t fetch_pa_addr(uint32_t *pagetable, uint32_t va) {
    uint32_t *pte;
    uint32_t pa;

    pte = walk(pagetable, va, 0);
    if (*pte == 0) {
        return 0;
    }
    if ((*pte & PTE_V) == 0) {
        return 0;
    }
    if ((*pte & PTE_U) == 0) {
        return 0;
    }
    pa = ((*pte >> 10) << 12);
    return pa;
}


// Copies len amount of bytes to dst from virtual address srcaddr
// in a given pagetable. Returns 0 on success and -1 for failure
int copyto(uint32_t *pagetable, char *dst, uint32_t srcaddr, uint32_t len) {
    uint32_t n, va, pa;

    while (len > 0) {
        va = (srcaddr & ~(PGESIZE - 1));
        pa = fetch_pa_addr(pagetable, va);
        if (pa == 0) {
            return -1;
        }
        n = PGESIZE - (srcaddr - va);
        if (n > len) {
            n = len;
        }
        memmove(dst, (void *)(pa + (srcaddr - va)), n);

        len -= n;
        dst += n;
        srcaddr = va + PGESIZE;
    }
    return 0;
}

// Copy len amount of bytes to dstaddr from src in a given pagetable
// returns 0 on success and -1 for failure
int copyout(uint32_t *pagetable, char *src, uint32_t dstaddr, uint32_t len) {
    uint32_t n, va, pa;

    while(len > 0) {
        va = (dstaddr & ~(PGESIZE - 1));
        pa = fetch_pa_addr(pagetable, va);
        if (pa == 0) {
            return 0;
        }
        n = PGESIZE - (dstaddr - va);
        if (n > len) {
            n = len;
        }
        memmove((void *)(pa + (dstaddr - va)), src, n);

        len -= n;
        src += n;
        dstaddr = va + PGESIZE;
    }
    return 0;
}


