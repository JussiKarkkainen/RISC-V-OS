#ifndef PAGING_H
#define PAGING_H

#include "arch/riscv/mem.S"
#include <stdint.h>

extern uint32_t HEAP_START; 
extern uint32_t TEXT_START;
extern uint32_t RODATA_START; 
extern uint32_t DATA_START;
extern uint32_t BSS_START; 
extern uint32_t KERNEL_STACK_START;
extern uint32_t HEAP_SIZE;

#define PTE_V 1
#define PTE_RX 0x6
#define PTE_X 0x3
#define PTE_W 0x2
#define PGEOFFSET 12
#define VPNMASK 0x3ff
#define PGESIZE (1 << 12)


#define UART0 0x10000000
#define VIRTIO0 0x10001000
#define PLIC 0xc000000
#define PLICSIZE 0x2000

#endif
