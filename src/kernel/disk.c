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
    
    // 1. Reset the device
    *(base_addr + DISK_STATUS) = 0;

    // 2. Set ACKNOWLEDGE status bit to status reg
    *(base_addr + DISK_STATUS) |= ACKNOWLEDGE_STATUS;

    // 3. Set DRIVER status bit to status reg
    *(base_addr + DISK_STATUS) |= DISK_DRIVER;

    // 4. Read device features from host_features register


}

void virtio_disk_intr(void) {
}
