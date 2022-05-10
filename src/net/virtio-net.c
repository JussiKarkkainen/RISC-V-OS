#include "virtio-net.h"
#include "pcie.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../kernel/disk.h"
#include "../libc/include/stdio.h"
#include <stdint.h>

#define R(r) ((volatile uint32_t *)(PCIE_MMIO_BASE + (r)))

struct Virtio_net {
        
    struct spinlock net_lock;

}__attribute__((aligned (PGESIZE))) net;


void virtio_net_init(void) {
    
    initlock(&net.net_lock, "net_lock");
    pcie_init();
     
    struct virtio_pci_common_cfg *net_common_cfg = (struct virtio_net_device *)PCIE_MMIO_BASE;
    

    // Device initialization described in virtio specification
    net_common_cfg->device_status = VIRTIO_DEV_RESET;
    net_common_cfg->device_status = VIRTIO_DEV_STATUS_ACKNOWLEDGE;
    net_common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER;
    
    uint32_t features = net_common_cfg->device_status;
    features &= ~(1 << VIRTIO_NET_F_CSUM);
    net_common_cfg->device_status = features;
    
    net_common_cfg->device_status |= VIRTIO_DEV_STATUS_FEATURES_OK;
    
    uint32_t device_status = net_common_cfg->device_status;
    kprintf("device_status %p\n", device_status);

    // Setupt queue 0 (receiveq1) and 1 (transmitq1)
    net_common_cfg->queue_select = 0;
    uint16_t queue_size = net_common_cfg->queue_size;
    kprintf("queue_size %p\n", queue_size);
    
     

}


int virtio_net_send(int payload_size) {
    
    int size = size + sizeof(virtio_net_hdr);

    if (size > 1526) {
        kprintf("packet size is too big %p\n", size);
        return 0;
    }
    
    virtio_net_header net_hdr;
    net_hdr.flags = VIRTIO_NET_HDR_F_NEEDS_CSUM;
    net_hdr.gso_type = 0;
    net_header.csum_start = 0;
    net_header.csum_offset = size;




}


int virtio_net_recv() {
}
