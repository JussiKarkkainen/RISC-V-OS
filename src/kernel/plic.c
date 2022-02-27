#include "process.h"

#define PLIC 0xc000000
#define PLIC_CLAIM_OFFSET 0x201004
#define PLIC_ENABLE_OFFSET

// https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
// Uarts intr_id = 10. 

void plic_init(void) {
    
}

void plic_init_hart(void) {
    int hart_id = which_cpu();

    // Set enable bit for this hart
    (PLIC + PLIC_ENABLE_OFFSET + hart_id * 0x100) = 
}

int plic_read(void) {
    int hart_id = which_cpu();
    // get interrupt ids from plic
    intr_id = (PLIC + PLIC_CLAIM_OFFSET + hart_id * 0x2000);
    return intr_id;
}

void plic_finished(int intr_id) {
    int hart_id = which_cpu();
    (PLIC + PLIC_CLAIM_OFFSET + hart_id * 0x2000) = intr_id;
}
