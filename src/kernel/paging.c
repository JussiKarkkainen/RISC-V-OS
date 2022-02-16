#include "pmm.c"
#include <stdint.h>
#include "paging.h"


extern uint32_t HEAP_START, TEXT_START, RODATA_START, DATA_START,
                BSS_START, KERNEL_STACK_START;


//typedef uint32_t *pagetable; // Pointer to a pagetable that contains 1024 PTEs
uint32_t *kernel_pagetable;


// create the kernel pagetable
uint32_t *kpagemake(void) {

    uint32_t *kpage = zalloc(1);

    // Create a virtual memory map
    kmap(kpage, UART0, UART0, pgsize, PTE_R | PTE_W);

    kmap(kpage, VIRTIO0);

    kmap(kpage, PLIC, PLIC);

    kmap(kpage, &HEAP_START, &HEAP_START);

    kmap(kpage, &TEXT_START, &HEAP_START);

    kmap(kpage, &RODATA_START, &RODATA_START);

    kmap(kpage, &DATA_START, &DATA_START);

    kmap(kpage, &BSS_START, &BSS_START);

    kmap(kpage, &KERNEL_STACK_START, &KERNEL_STACK_START);

    return kpage;
}


// Initializes the kernel page table
void kpage_init(void) {
    kpagetable = kpagemake();
}


// Write address of kernel/root pagetable to satp reg and flush TLBs
void init_paging() {
    satp_write(kpagetable);
    flush_tlb();
}

static inline void satp_write(uint32_t kpage) {
    asm volatile("csrw %0, satp", : : "r" (kpage));
}


// Check if valid bit is set in page table entry
bool is_valid(uint32_t pte) {
    if (pte & PTE_V)
        return true;
    else
        return false;
}

// Check if page table entry is a leaf
bool is_leaf(uint32_t pte) {
    // pte is leaf if any of the rx bits are set
    return (pte & PTE_RX); 
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

// 
int map(uint32_t *kpage, uint32_t vir_addr, uint32_t phy_addr, uint32_t size, int permissions) {

    uint32_t *pte;
    uint32_t last;
    uint32_t vir;

    if (size > 0) {
        last = vir_addr + size - 1;
        vir = vir_addr;

        while(1) {
            pte = walk(kpage, vir_addr, 1);
            if (pte == 0)
                return -1;
            if (*pte & PTE_V == 0) {
                

            }
    
        
        }
}







// Mapping a virtual address to a physical address
// root = address to kernel pagetable
int kmap(uint32_t *root, uint32_t viraddr, uint32_t, phyaddr, int bits, int level){
    
    // Check if READ, WRITE, and EXECUTE bits are set
    if (bits & 0xe != 0) {

        // Get the virtual page number
        // VPN[0] = bits 21-12, VPN[1] = bits 31-22.
        uint32_t vpn[2] = {((viraddr >> 22) & 0x3ff),  ((viraddr >> 12) & 0x3ff)};

        // Get physical page number
        // Otherwise same except PPN[1] is 12 bits instead of 10
        long long ppn[2] = {((viraddr >> 22) & 0xfff), ((viraddr >> 12) & 0xfff)};
        
        uint32_t v = *root[vpn[2]];

               
                uint32_t page = zalloc(1);
                 

            }
        }   
    }   
}


int kunmap() {
}


int translate_to_phy(uint32_t *root, uint32_t viraddr) {

    // Get the virtual page number
    // VPN[0] = bits 21-12, VPN[1] = bits 31-22.
    uint32_t vpn[2] = {((viraddr >> 22) & 0x3ff),  ((viraddr >> 12) & 0x3ff)};

    // Get physical page number
    // Otherwise same except PPN[1] is 12 bits instead of 10
    long long ppn[2] = {((viraddr >> 22) & 0xfff), ((viraddr >> 12) & 0xfff)};
    

    uint32_t *v = *root[vpn[2]];

    for (int i = 2; 1 > 0; i--) {
        
        if (is_valid(v)) {
            break;
        }
        
        else if (is_leaf(v)) {
                 

        }
    
    }   
    
}


