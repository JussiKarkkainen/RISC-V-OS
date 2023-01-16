#include "process.h"
#include "plic.h"
#include "paging.h"

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
<<<<<<< HEAD
    *(uint32_t*)PLIC_SENABLE(hart_id) = (1 << UART_IRQ) | (1 << VIRTIO_IRQ);
    // Set priority threshold 
    *(uint32_t*)PLIC_SPRIORITY(hart_id) = 0;
=======
    *(uint32_t *)PLIC_SENABLE(hart_id) = (1 << UART_IRQ) | (1 << VIRTIO_IRQ);
    // Set priority threshold 
    *(uint32_t *)PLIC_SPRIORITY(hart_id) = 0;
>>>>>>> origin/prod
}

int plic_read(void) {
    int hart_id = which_cpu();
    // get interrupt ids from plic
<<<<<<< HEAD
    int intr_id = *(uint32_t*)PLIC_SCLAIM(hart_id);
=======
    int intr_id = *(uint32_t *)PLIC_SCLAIM(hart_id);
>>>>>>> origin/prod
    return intr_id;
}

void plic_finished(int intr_id) {
    int hart_id = which_cpu();
<<<<<<< HEAD
    *(uint32_t*)PLIC_SCLAIM(hart_id) = intr_id;
=======
    *(uint32_t *)PLIC_SCLAIM(hart_id) = intr_id;
>>>>>>> origin/prod
}
