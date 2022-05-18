#include "virtio-net.h"
#include "pcie.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../kernel/disk.h"
#include "../libc/include/stdio.h"
#include <stdint.h>

#define R(r) ((volatile uint32_t *)(PCIE_MMIO_BASE + (r)))

struct Virtio_net {
        
    struct spinlock net_lock;
    
    struct net_avail *avail;
    struct net_used *used;
    struct disk_desc *desc;
    
    char free[8];

}__attribute__((aligned (PGESIZE))) net;


void virtio_net_init(void) {
    
    initlock(&net.net_lock, "net_lock");
    pcie_init();
     
    struct virtio_pci_common_cfg *net_common_cfg = (struct virtio_net_device *)PCIE_MMIO_BASE;
    

    // Device initialization described in virtio specification
    net_common_cfg->device_status = VIRTIO_DEV_RESET;
    net_common_cfg->device_status = VIRTIO_DEV_STATUS_ACKNOWLEDGE;
    net_common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER;
    
    uint32_t features = net_common_cfg->device_status;
    features &= ~(1 << VIRTIO_NET_F_CSUM);
    net_common_cfg->device_status = features;
    
    net_common_cfg->device_status |= VIRTIO_DEV_STATUS_FEATURES_OK;
    
    uint32_t device_status = net_common_cfg->device_status;
    kprintf("device_status %p\n", device_status);

    // Setupt queue 0 (receiveq1) and 1 (transmitq1)
    for (int i = 0; i <= 2; i++) {
        net_common_cfg->queue_select = i;
        uint16_t queue_size = net_common_cfg->queue_size;
        uint32_t gueue_addr = kalloc(gueue_size);                           // Check which version of kalloc is used
        memset(gueue_addr, 0, queue_size);
        net_common_cfg->queue_addr = (gueue_addr / PGESIZE);       // Find where queue_addr is located
        net_common_cfg->gueue_enable = 1;

        // Descriptor table
        virt = (uint64_t)kalloc(16 * qsize);
        vq.desc = (struct VirtqDescriptor *)virt;
        priv->common_cfg->queue_desc = mmu_translate(kernel_mmu_table, virt);
        
        // Driver ring (aka available ring)
        virt = (uint64_t)kalloc(6 + 2 * qsize);
        vq.driver = (struct VirtqAvail *)virt;
        priv->common_cfg->queue_driver = mmu_translate(kernel_mmu_table, virt);
        
        // Device ring (aka used ring)
        virt = (uint64_t)kalloc(6 + 8 * qsize);
        vq.device = (struct VirtqUsed *)virt;
        priv->common_cfg->queue_device = mmu_translate(kernel_mmu_table, virt);
        
        // Enable the queue (AFTER setting it up!)
        priv->common_cfg->queue_enable = 1;
        
        // Make the device LIVE
        priv->common_cfg->device_status |= VIRTIO_DEV_STATUS_DRIVER_OK;
    } 



}

int alloc_desc(void) {
    for (int i = 0; i < NUM; i++) {
        if (net.free[i]) {
            net.free[i] = 0;
            return i;
        }
    }
    return -1;
}

int virtionet_send_packet(uint32_t *payload, unsigned int size) {
    
    int size = size + sizeof(virtio_net_hdr);

    if (size > 1526) {
        kprintf("packet size is too big %p\n", size);
        return 0;
    }

    char buffer[size + sizeof(virtio_net_header)];
    
    virtio_net_header net_hdr = &buffer;
    net_hdr.flags = VIRTIO_NET_HDR_F_NEEDS_CSUM;
    net_hdr.gso_type = VIRTIO_NET_HDR_GSO_NONE;
    net_header.csum_start = 0;
    net_header.csum_offset = size;

    memcpy(buffer[sizeof(virtio_net_header)], payload, size);
    virtio_send_buffer(buffer, size + sizeof(net_header));

}


int virtio_net_recv() {



}
