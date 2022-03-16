#ifndef DISK_H
#define DISK_H

#include <stdint.h>

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
#define DISK_BLK_F_RO 5
#define DISK_BLK_F_SCSI 7
#define DISK_BLK_F_CONFIG_WCE 11
#define DISK_BLK_F_MQ 12
#define DISK_F_ANY_LAYOUT 27
#define DISK_RING_F_INDIRECT_DESC 28
#define DISK_RING_F_EVENT_IDX 29
#define DISK_FEATURES_OK 8
#define DISK_DRIVER_OK 4

#define NUM 8
#define BUFFER_SIZE 512
#define NUMBUF 30

#define DISK_BLOCK_WRITE 1
#define DISK_BLOCK_READ 0

#define DESC_NEXT 1
#define DESC_WRITE 2

struct disk_used_elem {
    uint32_t id;
    uint32_t len;
};

struct disk_used {
    uint16_t flags;
    uint16_t idx;
    struct disk_used_elem ring[8]};
};

struct buffer {
    int valid;
    int busy
    int changed;
    int disk;
    unsigned int dev;
    unsigned int blockno;
    struct sleeplock lock;
    unsigned int refcount;
    struct buffer *prev;
    struct buffer *next;
    char data[BUFFER_SIZE];
};

// Struct describing the format of the first 
// descriptor in a disk request
struct disk_block_req {
    uint32_t type;
    uint32_t reserved;
    uint32_t sector;
}

struct disk_avail {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[NUM];
    uint16_t unused;
}

void virtio_disk_intr(void);
int alloc_descriptor(void);
void free_descriptor(int i);
void disk_read_write(struct buffer *buf, int write);

#endif
