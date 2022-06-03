#include "virtio-net.h"
#include "pcie.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../kernel/disk.h"
#include "../libc/include/stdio.h"
#include <stdint.h>


void virtio_net_init(void) {

    initlock(&net.net_lock, "net_lock");
    pcie_init();

    struct virtio_pci_common_cfg *net_common_cfg = (struct virtio_net_device *)PCIE_MMIO_BASE;

    // Reset device
    net_common_cfg->device_status = VIRTIO_DEV_RESET;
    // Set ACKNOWLEDGE status bit: the guest OS has noticed the device
    net_common_cfg->device_status = VIRTIO_DEV_STATUS_ACKNOWLEDGE;
    // Set DRIVER status bit: the guest OS knows how to drive the device.
    net_common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER;

    // Read feature bits and nd write the subset of feature bits understood by the OS and driver to the device. 
    uint32_t features = net_common_cfg->device_feature;
    features &= ~(1 << VIRTIO_NET_F_CSUM);
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

}



void virtio_send_buffer(char *buffer, int size) {
}


int virtio_net_send_packet(uint32_t *payload, unsigned int size) {
}

int virtio_net_recv(void) {
}


