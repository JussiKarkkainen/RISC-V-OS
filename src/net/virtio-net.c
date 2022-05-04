#include "virtio-net.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../libc/include/stdio.h"
#include <stdint.h>

#define VIRT32_HIGH_PCIE_MMIO_BASE  0x30000000ULL

#define R(r) ((volatile uint32_t *)(VIRT32_HIGH_PCIE_MMIO_BASE + (r))) 

#define PCIE_ADDR 0x000
#define PCIE_SUBSYSTEM_ID 0x2c

uint32_t *phy_addr = (VIRT32_HIGH_PCIE_MMIO_BASE + (0x00 << 20 | 0x01 << 15 | 0x0 << 12));

// https://wiki.osdev.org/PCI#The_PCI_Bus
struct pci_config {
    uint32_t device_id;
    uint32_t status;
    uint32_t class_code;
    uint32_t BIST;
    uint32_t BAR0;
    uint32_t BAR1;
    uint32_t BAR2;
    uint32_t BAR3;
    uint32_t BAR4;
    uint32_t BAR5;
    uint32_t cardbus;
    uint32_t susbystem_id;
    uint32_t ROM_base_addr;
    uint32_t reserved;
    uint32_t reserved:
    uint32_t latency;
}

struct Virtio_net {
        
    struct spinlock net_lock;

}__attribute__((aligned (PGESIZE))) net;


void virtio_net_init(void) {
   
    initlock(&net.net_lock, "net_lock");
    kprintf("phy_addr %p\n", *phy_addr); 
    kprintf("mcfg %p\n", *R(PCIE_SUBSYSTEM_ID));



}


int virtio_net_send() {
}


int virtio_net_recv() {
}
