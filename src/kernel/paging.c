#include "pmm.c"
#include <stdint.h>

#define PTE_V 1
#define PTE_RX 0x6

// Mapping a virtual address to a physical address

int map(uint32_t *root, uint32_t viraddr, uint32_t, phyaddr, int bits, int level){
    
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



int unmap() {
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

