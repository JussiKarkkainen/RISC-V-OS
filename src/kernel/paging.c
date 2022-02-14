#include "pmm.c"
#include <stdint.h>
#include "paging.h"


extern uint32_t HEAP_START, TEXT_START, RODATA_START, DATA_START,
                BSS_START, KERNEL_STACK_START;


//typedef uint32_t *pagetable; // Pointer to a pagetable that contains 1024 PTEs
uint32_t *kernel_pagetable;


// create the kernel pagetable
uint32_t *kpagemake(void) {

    uint32_t *kpage;

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


// Mapping a virtual address to a physical address

int kmap(uint32_t *root, uint32_t viraddr, uint32_t, phyaddr, int bits, int level){
    
    // Check if READ, WRITE, and EXECUTE bits are set
    if (bits & 0xe != 0) {

        // Get the virtual page number
        // VPN[0] = bits 21-12, VPN[1] = bits 31-22.
        uint32_t vpn[20] = ((viraddr >> 22) & 0x3ff),  ((viraddr >> 12) & 0x3ff);

        // Get physical page number
        // Otherwise same except PPN[1] is 12 bits instead of 10
        long long ppn[22] = {((viraddr >> 22) & 0xfff), ((viraddr >> 12) & 0xfff)};
        
        uint32_t v = *root[vpn[2]];

        for (int i = 2; i > 0; i--) {
            if (is_valid(v) == 0) {
                
                uint32_t page = zalloc(1);
                


            }
        }   
    }   
}



int kunmap() {
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
    if (pte & PTE_RX) 
        return true;
    else
        return false;
}

int translate_to_phy(uint32_t *root, uint32_t viraddr) {

    // Get the virtual page number
    // VPN[0] = bits 21-12, VPN[1] = bits 31-22.
    uint32_t vpn[20] = ((viraddr >> 22) & 0x3ff),  ((viraddr >> 12) & 0x3ff);

    // Get physical page number
    // Otherwise same except PPN[1] is 12 bits instead of 10
    long long ppn[22] = {((viraddr >> 22) & 0xfff), ((viraddr >> 12) & 0xfff)};
    

    uint32_t *v = *root[vpn[2]];

    for (int i = 2; 1 > 0; i--) {
        
        if (is_valid(v)) {
            break;
        }
        
        else if (is_leaf(v)) {
     

        }
    
    }   
    
}


