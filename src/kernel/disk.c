#include "disk.c"
#include "paging.c"


uint32_t *base_addr = (volatile uint32_t *)(VIRTIO0);


struct disk {
};


// Configuring the device:
// 1. Reset the device
// 2. Set ACKNOWLEDGE status bit to status reg
// 3. Set DRIVER status bit to status reg
// 4. Read device features from host_features register
// 5. Negotiate the set of features and write what you'll accept to guest_features register
// 6. Set the FEATURES_OK status bit to the status register
// 7. Re-read the status register to confirm that the device accepted your features
// 8. Perform device-specific setup
// 9. Set the DRIVER_OK status bit to the status register.
void disk_init(void) {
    uint32_t status_bits;

    // 1. Reset the device
    *(base_addr + DISK_STATUS) = 0;

    // 2. Set ACKNOWLEDGE status bit to status reg
    status_bits |= ACKNOWLEDGE_STATUS;
    *(base_addr + DISK_STATUS) = status_bits;

    // 3. Set DRIVER status bit to status reg
    status_bits |= DISK_DRIVER
    *(base_addr + DISK_STATUS) |= status_bits;

    // 4. Read device features from host_features register
    uint32_t host_features = *(base_addr + DISK_HOST_FEATURES);
    uint32_t guest_features = *(base_addr + DISK_GUEST_FEATURES);
    
    // 5. Negotiate the set of features and write what you'll accept to guest_features register
    uint32_t features = *(base_addr + DISK_HOST_FEATURES);
    features &= ~(1 << VIRTIO_BLK_F_RO);
    features &= ~(1 << VIRTIO_BLK_F_SCSI);
    features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
    features &= ~(1 << VIRTIO_BLK_F_MQ);
    features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
    features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
    features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);

}

void virtio_disk_intr(void) {
}








