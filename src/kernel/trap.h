#ifndef TRAP_H
#define TRAP_H

#include <stdint.h>

#define SSTATUS_SPP (1 << 8)
#define SSTATUS_SIE (1 << 1)
#define SSTATUS_SPIE (1 << 5)
#define INTERRUPT_BIT (1 << 31)
#define EXT_INTERRUPT 0xff
#define SOFTWARE_INTR 0x80000001
#define CLEAR_SIP_SSIP ~2
#define UART_INTR 10
#define VIRTIO_DISK 1
#define SSTATUS_SIE_CLEAR 0xFFFFFFFD

struct spinlock tickslock;
unsigned int ticks;

int handle_device_intr(void);
void timer_interrupt(void);
void utrap(void);
void utrapret(void);
void ktrap(void);
void init_ktrapvec(void);
void init_trapvec(void);

#endif
