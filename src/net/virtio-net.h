#ifndef NET_H
#define NET_H

#include <stdint.h>

#define VIRT_PCIE_ECAM 0x30000000


// https://wiki.osdev.org/PCI#The_PCI_Bus
typedef volatile struct {
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
    uint32_t reserved1;
    uint32_t reserved2;
    uint32_t latency;
} pci_config;

void virtio_net_init(void);

int virtio_net_send();
int virtio_net_recv();

#endif
