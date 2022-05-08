#include "disk.h"
#include "paging.h"
#include "locks.h"
#include "process.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"

#define R(r) ((volatile uint32_t *)(VIRTIO0 + (r)))

struct disk {

    char pages[2 * PGESIZE];

    struct spinlock disk_lock;
    
    struct disk_block_req ops[NUM];
    struct disk_avail *avail;
    struct disk_used *used;
    struct disk_desc *desc;
    
    char free[8];
    uint16_t used_idx;

    struct {
        struct buffer *b;
        char status;
    }info[NUM];

}__attribute__((aligned (PGESIZE))) disk;

void disk_init(void) {
    uint32_t status_bits = 0;

    initlock(&disk.disk_lock, "disk");

    if(*R(DISK_MAGIC_VAL) != 0x74726976 ||
       *R(DISK_VERSION) != 1 ||
       *R(DISK_DEVICEID) != 2 ||
       *R(DISK_VENDORID) != 0x554d4551){
        
        panic("disk not found");
    }
    
    // 1. Reset the device
    *R(DISK_STATUS) = 0;

    // 2. Set ACKNOWLEDGE status bit to status reg
    status_bits |= ACKNOWLEDGE_STATUS;
    *R(DISK_STATUS) = status_bits;

    // 3. Set DRIVER status bit to status reg
    status_bits |= DRIVER_STATUS;
    *R(DISK_STATUS) |= status_bits;

    // 4. Negotiate the set of features and write what you'll accept to host_features register
    uint32_t features = *R(DISK_HOST_FEATURES);
    features &= ~(1 << DISK_BLK_F_RO);
    features &= ~(1 << DISK_BLK_F_SCSI);
    features &= ~(1 << DISK_BLK_F_CONFIG_WCE);
    features &= ~(1 << DISK_BLK_F_MQ);
    features &= ~(1 << DISK_F_ANY_LAYOUT);
    features &= ~(1 << DISK_RING_F_EVENT_IDX);
    features &= ~(1 << DISK_RING_F_INDIRECT_DESC);
    *R(DISK_HOST_FEATURES) = features;

    // Set status bit to indicate we're ready
    status_bits |= DISK_FEATURES_OK;
    *R(DISK_STATUS) = status_bits;

    status_bits = DISK_DRIVER_OK;
    *R(DISK_STATUS) = status_bits;

    // Set pagesize
    *R(DISK_GUEST_PAGE_SIZE) = PGESIZE;
    
    // Initialize queue 0
    *R(DISK_QUEUE_SEL) = 0;
    uint32_t max = *R(DISK_QUEUE_NUM_MAX);
    
    if (max == 0) {
        panic("no queue 0, disk");
    }
    if (max < NUM) {
        panic("queue too short, disk");
    }

    *R(DISK_QUEUE_NUM) = NUM;   // Number of descriptors
    memset(disk.pages, 0, sizeof(disk.pages));
    *R(DISK_QUEUE_PFN) = ((uint32_t)disk.pages >> 12);
    
    disk.desc = (struct disk_desc *) disk.pages;
    disk.avail = (struct disk_avail *)(disk.pages + NUM * sizeof(struct disk_desc));
    disk.used = (struct disk_used *) (disk.pages + PGESIZE);


    // descriptors start unused
    for (int i=0; i<8; i++) {
        disk.free[i] = 1;
    }
}

void virtio_disk_intr(void) {
    
    acquire_lock(&disk.disk_lock);

    // Tell device we've seen the interrupt and it can send another one
    *R(DISK_INTR_ACK) = *R(DISK_INTR_STATUS) & 0x3;

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
    }
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
static void
free_chain(int i)
{
  while(1){
    int flag = disk.desc[i].flags;
    int nxt = disk.desc[i].next;
    free_descriptor(i);
    if(flag & VRING_DESC_F_NEXT)
      i = nxt;
    else
      break;
  }
}

void disk_read_write(struct buffer *buf, int write) {
   
    uint64_t sector = buf->blockno * (BUFFER_SIZE / 512);
    acquire_lock(&disk.disk_lock);

    int idx[3];
    while (1) {
        if (alloc3_descriptors(idx) == 0) {
            break;
        }
        sleep(&disk.free[0], &disk.disk_lock);
    }

    // Format descriptors
    struct disk_block_req *buf0 = &disk.ops[idx[0]];


//    struct disk_block_req buf0;

    if (write) {
        buf0->type = DISK_BLOCK_WRITE;
    }
    else {
        buf0->type = DISK_BLOCK_READ;
    
    buf0->reserved = 0;
    buf0->sector = sector;
/*
    disk.desc[idx[0]].addr = fetch_kpa((uint32_t)&buf0);
    disk.desc[idx[0]].len = sizeof(buf0);
    disk.desc[idx[0]].flags = DESC_NEXT;
    disk.desc[idx[0]].next = idx[1];

*/

    disk.desc[idx[0]].addr = (uint32_t) buf0;
    disk.desc[idx[0]].len = sizeof(struct disk_block_req);
    disk.desc[idx[0]].flags = DESC_NEXT;
    disk.desc[idx[0]].next = idx[1];

    disk.desc[idx[1]].addr = (uint32_t) buf->data;
    disk.desc[idx[1]].len = BLOCK_SIZE;


    if(write) {
        disk.desc[idx[1]].flags = 0; // device reads b->data
    }
    else {
        disk.desc[idx[1]].flags = DESC_WRITE; // device writes b->data
    }
    disk.desc[idx[1]].flags |= DESC_NEXT;
    disk.desc[idx[1]].next = idx[2];

    disk.info[idx[0]].status = 0; // device writes 0 on success
    disk.desc[idx[2]].addr = (uint32_t) &disk.info[idx[0]].status & 0xffffffff;
    disk.desc[idx[2]].len = 1;
    disk.desc[idx[2]].flags = DESC_WRITE; // device writes the status
    disk.desc[idx[2]].next = 0; 

    buf->disk = 1;
    disk.info[idx[0]].b = buf;

    // tell the device the first index in our chain of descriptors.
    disk.avail->ring[disk.avail->idx % NUM] = idx[0];

    __sync_synchronize();

    // tell the device another avail ring entry is available.
    disk.avail->idx += 1; // not % NUM ...

    __sync_synchronize();

    *R(DISK_QUEUE_NOTIFY) = 0; // value is queue number

    // Wait for virtio_disk_intr() to say request has finished.
    while(buf->disk == 1) {
        kprintf("sleep\n");
        sleep(buf, &disk.disk_lock);
    }

    disk.info[idx[0]].b = 0;
    free_chain(idx[0]);

    release_lock(&disk.disk_lock);
    }
}


