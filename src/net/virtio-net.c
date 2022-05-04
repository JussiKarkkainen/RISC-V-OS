#include "virtio-net.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../libc/include/stdio.h"
#include <stdint.h>

#define VIRT32_HIGH_PCIE_MMIO_BASE  0x30000000ULL

#define R(r) ((volatile uint32_t *)(VIRT32_HIGH_PCIE_MMIO_BASE + (r))) 

#define PCIE_ADDR 0x000
#define PCIE_SUBSYSTEM_ID 0x2c

// Find the address of pci config space
static volatile pci_config *pci_config_regs = (VIRT_PCIE_ECAM + (0x00 << 20 | 0x01 << 15 | 0x0 << 12));


struct Virtio_net {
        
    struct spinlock net_lock;

}__attribute__((aligned (PGESIZE))) net;


uint32_t get_bar(int bar_num) {

    uint32_t bar_base = pcie_config_regs->BAR0;
    return *(bar_base + (bar_num * 4));

}
void virtio_net_init(void) {
   
    initlock(&net.net_lock, "net_lock");



}


int virtio_net_send() {
}


int virtio_net_recv() {
}
