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
     
    struct virtio_net_device *net_device = (struct virtio_net_device *)PCIE_MMIO_BASE;
    
    uint32_t magic_val = (uint32_t)*R(DISK_MAGIC_VAL);
//    kprintf("heelo\n", net_device->common_cfg->num_queues);
    kprintf("magic val %p\n", magic_val);

    uint16_t *num_queues = (uint16_t)net_device->common_cfg.num_queues;
    kprintf("net_device->num_queues %p\n", num_queues);
/*
    // Device initialization described in virtio specification
    net_device->common_cfg->device_status = VIRTIO_DEV_RESET;
    net_device->common_cfg->device_status = VIRTIO_DEV_STATUS_ACKNOWLEDGE;
    net_device->common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER;
    
    uint32_t features = net_device->common_cfg->device_status;
    features &= ~(1 << VIRTIO_NET_CSUM);
    net_device->common_cfg->device_status = features;
    
    net_device->common_cfg->device_status |= VIRTIO_DEV_STATUS_FEATURES_OK;

    // Setupt queue 0 (receiveq1) and 1 (transmitq1)
    net_device->common_cfg->queue_select = 0;
    uint16_t queue_size = net_device->common_cfg->queue_size;
    
*/

    


}


int virtio_net_send() {
}


int virtio_net_recv() {
}
