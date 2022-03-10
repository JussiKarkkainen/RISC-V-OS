#include "process.h"

#define PLIC 0xc000000
#define PLIC_CLAIM_OFFSET 0x201004
#define PLIC_ENABLE_OFFSET 0x2000
#define PLIC_THRESHOLD 0x2080
#define UART_IRQ 10
#define VIRTIO_IRQ 1

// https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
// UART intr_id = 10.
// VIRTIO intr_id = 1-8
// PCIE intr_id = 32-35


void plic_init(void) {
    *(uint32_t *)(PLIC + UART_IRQ * 4) = 1;
    *(uint32_t *)(PLIC + VIRTIO_IRQ * 4) = 1;
}

void plic_init_hart(void) {
    int hart_id = which_cpu();
    // Set enable bit for this hart
    *(uint32_t *)(PLIC + PLIC_ENABLE_OFFSET + hart_id * 0x100) = (1 << UART_IRQ) | (1 << VIRTIO_IRQ);
    // Set priority threshold 
    *(uint32_t *)(PLIC + PLIC_THRESHOLD + hart_id * 0x100) = 0;
}

int plic_read(void) {
    int hart_id = which_cpu();
    // get interrupt ids from plic
    intr_id = *(uint32_t *)(PLIC + PLIC_CLAIM_OFFSET + hart_id * 0x2000);
    return intr_id;
}

void plic_finished(int intr_id) {
    int hart_id = which_cpu();
    *(uint32_t *)(PLIC + PLIC_CLAIM_OFFSET + hart_id * 0x2000) = intr_id;
}
