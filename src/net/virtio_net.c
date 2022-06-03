#include "virtio-net.h"
#include "pcie.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../kernel/disk.h"
#include "../libc/include/stdio.h"
#include <stdint.h>

    
struct virtio_pci_common_cfg *net_common_cfg = (struct virtio_net_device *)PCIE_MMIO_BASE;

void virtio_net_init(void) {

    initlock(&net.net_lock, "net_lock");
    pcie_init();

    // Reset device
    net_common_cfg->device_status = VIRTIO_DEV_RESET;
    // Set ACKNOWLEDGE status bit: the guest OS has noticed the device
    net_common_cfg->device_status = VIRTIO_DEV_STATUS_ACKNOWLEDGE;
    // Set DRIVER status bit: the guest OS knows how to drive the device.
    net_common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER;

    // Read feature bits and nd write the subset of feature bits understood by the OS and driver to the device. 
    uint32_t features = net_common_cfg->device_feature;
    features &= ~(1 << VIRTIO_NET_F_CSUM);
    features &= ~(1 << VIRTIO_NET_F_MAC);
    net_common_cfg->device_feature_select = features;
    
    // Set the FEATURES_OK status bit. 
    net_common_cfg->device_status |= VIRTIO_DEV_STATUS_FEATURES_OK;

    // Re-read device status to ensure the FEATURES_OK bit is still set.
    if (net_common_cfg->device_status && FEATURES_OK != 1) {
        panic("Features not supported, device unusable");
    }

    // Configure virtqueues
    init_queue(0);          // receiveq
    init_queue(1);          // transmitq

    // Set the DRIVER_OK status bit. At this point the device is “live”
    net_common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER_OK;

}



void init_queue(int index) {

    net_common_cfg->queue_select = index;
    uint32_t queue_size = net_common_cfg->queue_size;
    
    // Descriptor table
    uint32_t *desc = kalloc(16 * queue_size);
    net_common_cfg->queue_desc = desc;

    // Driver/available ring
    uint32_t *driver = kalloc(6 + 2 *queue_size);
    net_common_cfg->queue_driver = driver;

    // Device/used ring
    uint32_t *device = kalloc(6 + 8 * queue_size);
    net_common_cfg->queue_device = device;
    
    // Enable queue
    net_common_cfg->queue_enable = 1;
}


int virtio_net_send_packet(uint32_t *payload, unsigned int size) {
    
    int size = size + sizeof(virtio_net_hdr);

    if (size > 1526) {
        kprintf("packet size is too big %p\n", size);
        return 0;
    }

    char buffer[size + sizeof(virtio_net_header)];
    
    struct virtio_net_header net_hdr = &buffer;
    net_hdr.flags = VIRTIO_NET_HDR_F_NEEDS_CSUM;
    net_hdr.gso_type = VIRTIO_NET_HDR_GSO_NONE;
    net_header.csum_start = 0;
    net_header.csum_offset = size;

    memcpy(buffer[sizeof(virtio_net_header)], payload, size);
    virtio_send_buffer(buffer, size + sizeof(net_header));

}

int virtio_net_recv(void) {
}


