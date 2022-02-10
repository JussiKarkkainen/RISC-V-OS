#include "pmm.c"



// Mapping a virtual address to a physical address

int map(uint32_t *root, uint32_t viraddr, uint32_t, phyaddr, int bits, int level){
    
    // Check if READ, WRITE, and EXECUTE bits are set
    if (bits & 0xe != 0) {

        // Get the virtual page number
        // VPN[0] = bits 21-12, VPN[1] = bits 31-22.
        uint32_t vpn = (((viraddr >> 22) & 0x3ff) << 10) | ((viraddr >> 12) & 0x3ff);

        // Get physical page number
        // Otherwise same except PPN[1] is 12 bits instead of 10
        long long ppn = (((viraddr >> 22) & 0xfff) << 12) | ((viraddr >> 12) & 0xfff);

}



int unmap() {
}

int translate_to_phy() {
}

