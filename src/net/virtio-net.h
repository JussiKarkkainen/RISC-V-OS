#ifndef NET_H
#define NET_H

#include <stdint.h>

#define PCIE_MMIO_BASE 0x400000000

#define VIRTIO_PCI_CAP_COMMON_CFG 1     // Common configuration
#define VIRTIO_PCI_CAP_NOTIFY_CFG 2     // Notifications
#define VIRTIO_PCI_CAP_ISR_CFG 3        // ISR Status
#define VIRTIO_PCI_CAP_DEVICE_CFG 4     // Device specific configuration
#define VIRTIO_PCI_CAP_PCI_CFG 5        // PCI configuration access


struct virtio_pci_cap {
    uint8_t cap_vndr;   
    uint8_t cap_next;   
    uint8_t cap_len;    
    uint8_t cfg_type;   
    uint8_t bar;         // Which BAR to find it
    uint8_t padding[3]; 
    uint32_t offset;    
    uint32_t length;    
};


void virtio_net_init(void);

int virtio_net_send();
int virtio_net_recv();

#endif
