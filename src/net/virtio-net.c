#include "virtio-net.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../libc/include/stdio.h"
#include <stdint.h>


struct Virtio_net {
        
    struct spinlock net_lock;

}__attribute__((aligned (PGESIZE))) net;


void virtio_net_init(void) {
    
    initlock(&net.net_lock, "net_lock");

    struct virtio_net_device *net_device = (struct virtio_net_device *)PCIE_MMIO_BASE;

    // Device initialization described in virtio specification
    net_device->common_cfg->device_status = VIRTIO_DEV_RESET;
    net_device->common_cfg->device_status = VIRTIO_DEV_STATUS_ACKNOWLEDGE;
    net_device->common_cfg->device_status = VIRTIO_DEV_STATUS_DRIVER;

    // Read features
    net_device->common_cfg->queue_select = 0;
    uint16_t queue_size = net_device->common_cfg->queue_size;



    


}


int virtio_net_send() {
}


int virtio_net_recv() {
}
