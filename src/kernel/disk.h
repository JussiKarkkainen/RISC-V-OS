#ifndef DISK_H
#define DISK_H

// Offsets for disk driver
#define DISK_MAGIC_VAL 0x000
#define DISK_VERSION 0x004
#define DISK_DEVICEID 0x008
#define DISK_VENDORID 0x00c
#define DISK_HOST_FEATURES 0x010
#define DISK_HOST_FEATURES_SEL 0x014
#define DISK_GUEST_FEATURES 0x020
#define DISK_GUEST_FEATURES_SEL 0x024
#define DISK_GUEST_PAGE_SIZE 0x028
#define DISK_QUEUE_SEL 0x030
#define DISK_QUEUE_NUM_MAX 0x034
#define DISK_QUEUE_NUM 0x038
#define DISK_QUEUE_ALIGN 0x03c
#define DISK_QUEUE_PFN 0x040
#define DISK_QUEUE_NOTIFY 0x050
#define DISK_INTR_STATUS 0x060
#define DISK_INTR_ACK 0x064
#define DISK_STATUS 0x070
#define DISK_CONFIG 0x100

// Defenitions for setting bits
#define ACKNOWLEDGE_STATUS 1
#define DRIVER_STATUS 2


void virtio_disk_intr(void);

#endif
