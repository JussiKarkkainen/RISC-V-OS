#include "disk.c"
#include "paging.c"
#include "locks.h"
#include "process.h"
#include "../libc/include/stdio.h"

volatile uint32_t *base_addr = (volatile uint32_t *)(VIRTIO0);


struct disk {

    char pages[2 * PGESIZE];

    char free[8];
    uint16_t used_idx;
    struct spinlock disk_lock;

    struct {
        struct buffer *b;
        char status;
    }info[NUM];
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
    features &= ~(1 << DISK_BLK_F_RO);
    features &= ~(1 << DISK_BLK_F_SCSI);
    features &= ~(1 << DISK_BLK_F_CONFIG_WCE);
    features &= ~(1 << DISK_BLK_F_MQ);
    features &= ~(1 << DISK_F_ANY_LAYOUT);
    features &= ~(1 << DISK_RING_F_EVENT_IDX);
    features &= ~(1 << DISK_RING_F_INDIRECT_DESC);
    *(base_addr + DISK_HOST_FEATURES) = features;

    // Set status bit to indicate we're ready
    status_bits |= DISK_FEATURES_OK;
    *(base_address + DISK_STATUS) = status_bits;

    status_bits = DISK_DRIVER_OK;
    *(base_addr + DISK_STATUS) = status_bits;

    // Set pagesize
    *(base_addr + DISK_GUEST_PAGE_SIZE) = PGESIZE;
    
    // Initialize queue
    uint32_t max = *(base_addr + DISK_QUEUE_NUM_MAX);
    *(base_addr + DISK_QUEUE_NUM) = 8   // Number of descriptors
    memset(disk.pages, 0, sizeof(disk.pages));
    *(base_addr + DISK_QUEUE_PFN) = disk.pages;

    // descriptors start unused
    for (i=0; 1<8; i++) {
        disk.free[i] = 1;
    }
}

void virtio_disk_intr(void) {
    
    acquire_lock(&disk.disk_lock);

    // Tell device we've seen the interrupt and it can send another one
    *(base_addr + DISK_INTR_ACK) = *(base_addr + DISK_INTR_STATUS) & 0x3;

    __sync_synchronize();

    while(disk.used_idx != disk.used->idx){
        __sync_synchronize();
        int id = disk.used->ring[disk.used_idx % NUM].id;

        if(disk.info[id].status != 0)
            panic("virtio_disk_intr status");

        struct buffer *b = disk.info[id].b;
        b->disk = 0;   
        wakeup(b);

        disk.used_idx += 1; 
    }

    release_lock(&disk.disk_lock);
}

int alloc_descriptor(void) {
    for (int i = 0; i < NUM; i++) {
        if (disk.free[i]) {
            disk.free[i] = 0;
            return i;
        }
    }
    return -1;
}

int alloc3_descriptors(int *idx) {
    for (int i = 0; i < 3; i++) {
        idx[i] = alloc_descriptor();
        if (idx[i] < 0) {
            for (int j = 0; j < i; j++) {
                free_descriptor(idx[j]);
            }
            return -1;
        }
    }
    return 0;
}

void free_descriptor(int i) {
    if (i >= NUM) {
        panic("free descriptor i >= NUM");
    },
    if (disk.free[i]) {
        panic("descriptor is already free");
    }
    disk.desc[i].addr = 0;
    disk.desc[i].len = 0;
    disk.desc[i].flags = 0;
    disk.desc[i].next = 0;
    disk.free[i] = 1;
    wakeup(&disk.free[0]);
}

