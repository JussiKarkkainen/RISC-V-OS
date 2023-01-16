#include "disk.h"
#include "virtio.h"
#include "locks.h"
#include "filesys.h"
#include "process.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include "paging.h"

// the address of virtio mmio register r.
#define R(r) ((volatile uint32_t *)(VIRTIO0 + (r)))

<<<<<<< HEAD
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
=======
static struct disk {
 // memory for virtio descriptors &c for queue 0.
 // this is a global instead of allocated because it must
 // be multiple contiguous pages, which kalloc()
 // doesn't support, and page aligned.
  char pages[2*PGESIZE];
  struct VRingDesc *desc;
  uint16_t *avail;
  struct UsedArea *used;

  // our own book-keeping.
  char free[NUM];  // is a descriptor free?
  uint16_t used_idx; // we've looked this far in used[2..NUM].

  // track info about in-flight operations,
  // for use when completion interrupt arrives.
  // indexed by first descriptor index of chain.
  struct {
    struct buffer *b;
    char status;
  } info[NUM];
  
  struct spinlock vdisk_lock;
  
} __attribute__ ((aligned (PGESIZE))) disk;

void
disk_init(void)
{
  uint32_t status = 0;
>>>>>>> origin/prod

  initlock(&disk.vdisk_lock, "virtio_disk");

  if(*R(VIRTIO_MMIO_MAGIC_VALUE) != 0x74726976 ||
     *R(VIRTIO_MMIO_VERSION) != 1 ||
     *R(VIRTIO_MMIO_DEVICE_ID) != 2 ||
     *R(VIRTIO_MMIO_VENDOR_ID) != 0x554d4551){
    panic("could not find virtio disk");
  }
  
  status |= VIRTIO_CONFIG_S_ACKNOWLEDGE;
  *R(VIRTIO_MMIO_STATUS) = status;

  status |= VIRTIO_CONFIG_S_DRIVER;
  *R(VIRTIO_MMIO_STATUS) = status;

  // negotiate features
  uint32_t features = *R(VIRTIO_MMIO_DEVICE_FEATURES);
  features &= ~(1 << VIRTIO_BLK_F_RO);
  features &= ~(1 << VIRTIO_BLK_F_SCSI);
  features &= ~(1 << VIRTIO_BLK_F_CONFIG_WCE);
  features &= ~(1 << VIRTIO_BLK_F_MQ);
  features &= ~(1 << VIRTIO_F_ANY_LAYOUT);
  features &= ~(1 << VIRTIO_RING_F_EVENT_IDX);
  features &= ~(1 << VIRTIO_RING_F_INDIRECT_DESC);
  *R(VIRTIO_MMIO_DRIVER_FEATURES) = features;

  // tell device that feature negotiation is complete.
  status |= VIRTIO_CONFIG_S_FEATURES_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  // tell device we're completely ready.
  status |= VIRTIO_CONFIG_S_DRIVER_OK;
  *R(VIRTIO_MMIO_STATUS) = status;

  *R(VIRTIO_MMIO_GUEST_PAGE_SIZE) = PGESIZE;

  // initialize queue 0.
  *R(VIRTIO_MMIO_QUEUE_SEL) = 0;
  uint32_t max = *R(VIRTIO_MMIO_QUEUE_NUM_MAX);
  if(max == 0)
    panic("virtio disk has no queue 0");
  if(max < NUM)
    panic("virtio disk max queue too short");
  *R(VIRTIO_MMIO_QUEUE_NUM) = NUM;
  memset(disk.pages, 0, sizeof(disk.pages));
  *R(VIRTIO_MMIO_QUEUE_PFN) = ((uint32_t)disk.pages) >> PGEOFFSET;
  // *R(VIRTIO_MMIO_QUEUE_ALIGN) = PGSIZE; // patch proposed in github pulls, works without

  // desc = pages -- num * VRingDesc
  // avail = pages + 0x40 -- 2 * uint16, then num * uint16
  // used = pages + 4096 -- 2 * uint16, then num * vRingUsedElem

  disk.desc = (struct VRingDesc *) disk.pages;
  disk.avail = (uint16_t*)(((char*)disk.desc) + NUM*sizeof(struct VRingDesc));
  disk.used = (struct UsedArea *) (disk.pages + PGESIZE);

  for(int i = 0; i < NUM; i++)
    disk.free[i] = 1;

  // plic.c and trap.c arrange for interrupts from VIRTIO0_IRQ.
}

// find a free descriptor, mark it non-free, return its index.
static int
alloc_desc()
{
  for(int i = 0; i < NUM; i++){
    if(disk.free[i]){
      disk.free[i] = 0;
      return i;
    }
  }
  return -1;
}

// mark a descriptor as free.
static void
free_desc(int i)
{
  if(i >= NUM)
    panic("virtio_disk_intr 1");
  if(disk.free[i])
    panic("virtio_disk_intr 2");
  disk.desc[i].addr = 0;
  disk.free[i] = 1;
  wakeup(&disk.free[0]);
}

// free a chain of descriptors.
static void
free_chain(int i)
{
  while(1){
    free_desc(i);
    if(disk.desc[i].flags & VRING_DESC_F_NEXT)
      i = disk.desc[i].next;
    else
      break;
  }
}

static int
alloc3_desc(int *idx)
{
  for(int i = 0; i < 3; i++){
    idx[i] = alloc_desc();
    if(idx[i] < 0){
      for(int j = 0; j < i; j++)
        free_desc(idx[j]);
      return -1;
    }
<<<<<<< HEAD
    else {
        buf0->type = DISK_BLOCK_READ;
    } 
    
    buf0->reserved = 0;
    buf0->sector = sector;
/*
    disk.desc[idx[0]].addr = fetch_kpa((uint32_t)&buf0);
    disk.desc[idx[0]].len = sizeof(buf0);
    disk.desc[idx[0]].flags = DESC_NEXT;
    disk.desc[idx[0]].next = idx[1];

*/
=======
  }
  return 0;
}
>>>>>>> origin/prod

void
disk_read_write(struct buffer *b, int write)
{
  uint64_t sector = b->blockno * (BLOCK_SIZE / 512);

  acquire_lock(&disk.vdisk_lock);

  // the spec says that legacy block operations use three
  // descriptors: one for type/reserved/sector, one for
  // the data, one for a 1-byte status result.

<<<<<<< HEAD
    if(write) {
        disk.desc[idx[1]].flags = 1; // device reads b->data
    }
    else {
        disk.desc[idx[1]].flags = 0; // device writes b->data
    }
    disk.desc[idx[1]].flags |= DESC_NEXT;
    disk.desc[idx[1]].next = idx[2];

    disk.info[idx[0]].status = 0xff; // device writes 0 on success
    disk.desc[idx[2]].addr = (uint32_t) &disk.info[idx[0]].status & 0xffffffff;
    disk.desc[idx[2]].len = 1;
    disk.desc[idx[2]].flags = DESC_WRITE; // device writes the status
    disk.desc[idx[2]].next = 0; 

    buf->disk = 1;
    disk.info[idx[0]].b = buf;
=======
  // allocate the three descriptors.
  int idx[3];
  while(1){
    if(alloc3_desc(idx) == 0) {
      break;
    }
    sleep(&disk.free[0], &disk.vdisk_lock);
  }
  
  // format the three descriptors.
  // qemu's virtio-blk.c reads them.

  struct virtio_blk_outhdr {
    uint32_t type;
    uint32_t reserved;
    uint64_t sector;
  } buf0;

  if(write)
    buf0.type = VIRTIO_BLK_T_OUT; // write the disk
  else
    buf0.type = VIRTIO_BLK_T_IN; // read the disk
  buf0.reserved = 0;
  buf0.sector = sector;

  // buf0 is on a kernel stack, which is not direct mapped,
  // thus the call to kvmpa().
  // disk.desc[idx[0]].addr = (uint32)((uint64) kvmpa((uint32) &buf0)) & 0xffffffff;
  disk.desc[idx[0]].addr = fetch_kpa((uint32_t) &buf0);
  disk.desc[idx[0]].len = sizeof(buf0);
  disk.desc[idx[0]].flags = VRING_DESC_F_NEXT;
  disk.desc[idx[0]].next = idx[1];

  disk.desc[idx[1]].addr = ((uint32_t) b->data) & 0xffffffff; // XXX
  disk.desc[idx[1]].len = BLOCK_SIZE;
  if(write)
    disk.desc[idx[1]].flags = 0; // device reads b->data
  else
    disk.desc[idx[1]].flags = VRING_DESC_F_WRITE; // device writes b->data
  disk.desc[idx[1]].flags |= VRING_DESC_F_NEXT;
  disk.desc[idx[1]].next = idx[2];

  disk.info[idx[0]].status = 0;
  disk.desc[idx[2]].addr = ((uint32_t) &disk.info[idx[0]].status) & 0xffffffff; // XXX
  disk.desc[idx[2]].len = 1;
  disk.desc[idx[2]].flags = VRING_DESC_F_WRITE; // device writes the status
  disk.desc[idx[2]].next = 0;

  // record struct buf for virtio_disk_intr().
  b->disk = 1;
  disk.info[idx[0]].b = b;

  // avail[0] is flags
  // avail[1] tells the device how far to look in avail[2...].
  // avail[2...] are desc[] indices the device should process.
  // we only tell device the first index in our chain of descriptors.
  disk.avail[2 + (disk.avail[1] % NUM)] = idx[0];
  __sync_synchronize();
  disk.avail[1] = disk.avail[1] + 1;

  *R(VIRTIO_MMIO_QUEUE_NOTIFY) = 0; // value is queue number

  // Wait for virtio_disk_intr() to say request has finished.
  while(b->disk == 1) {
    sleep(b, &disk.vdisk_lock);
  }
>>>>>>> origin/prod

  disk.info[idx[0]].b = 0;
  free_chain(idx[0]);

  release_lock(&disk.vdisk_lock);
}

void
virtio_disk_intr()
{
  acquire_lock(&disk.vdisk_lock);

  while((disk.used_idx % NUM) != (disk.used->id % NUM)){
    __sync_synchronize();

    int id = disk.used->elems[disk.used_idx].id;

<<<<<<< HEAD
    // Wait for virtio_disk_intr() to say request has finished.
    kprintf("disk\n");
    while(buf->disk == 1) {
        sleep(buf, &disk.disk_lock);
    }

    disk.info[idx[0]].b = 0;
    free_chain(idx[0]);
    
    release_lock(&disk.disk_lock);
    
=======
    if(disk.info[id].status != 0)
      panic("virtio_disk_intr status");
    
    disk.info[id].b->disk = 0;   // disk is done with buf
    wakeup(disk.info[id].b);

    disk.used_idx = (disk.used_idx + 1) % NUM;
  }

  release_lock(&disk.vdisk_lock);
>>>>>>> origin/prod
}
