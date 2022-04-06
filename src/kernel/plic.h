#ifndef PLIC_H
#define PLIC_H

#define PLIC_CLAIM_OFFSET 0x201004
#define PLIC_ENABLE_OFFSET 0x2000
#define PLIC_THRESHOLD 0x2080
#define UART_IRQ 10
#define VIRTIO_IRQ 1

void plic_init(void);
void plic_init_hart(void);
int plic_read(void);
void plic_finished(int intr_id);

#endif
