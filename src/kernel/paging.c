#include "pmm.c"



// Mapping a virtual address to a physical address

int map(uint32_t *root, uint32_t viraddr, uint32_t, phyaddr, int bits, int level){
    // Check if READ, WRITE, and EXECUTE bits are set

    if (bits & 0xe != 0) {

        // Get the virtual page number
        // VPN[0] = bits 21-12, VPN[1] = bits 31-22.
        uint32_t vpn = (((viraddr >> 22) & 0x3ff) << 10) | ((viraddr >> 12) & 0x3ff);




}



int unmap() {
}

int translate_to_phy() {
}

