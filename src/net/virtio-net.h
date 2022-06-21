#ifndef NET_H
#define NET_H

#include <stdint.h>
#include "../kernel/disk.h"

#define PCIE_MMIO_BASE 0x40000000

#define VIRTIO_PCI_CAP_COMMON_CFG 1     // Common configuration
#define VIRTIO_PCI_CAP_NOTIFY_CFG 2     // Notifications
#define VIRTIO_PCI_CAP_ISR_CFG 3        // ISR Status
#define VIRTIO_PCI_CAP_DEVICE_CFG 4     // Device specific configuration
#define VIRTIO_PCI_CAP_PCI_CFG 5        // PCI configuration access


#define VIRTIO_DEV_RESET 0
#define VIRTIO_DEV_STATUS_ACKNOWLEDGE 1
#define VIRTIO_DEV_STATUS_DRIVER 2
#define VIRTIO_DEV_STATUS_FEATURES_OK 8

#define VIRTIO_NET_F_CSUM 0

// https://docs.oasis-open.org/virtio/virtio/v1.1/virtio-v1.1.pdf
struct virtio_pci_cap {
    uint8_t cap_vndr;   
    uint8_t cap_next;   
    uint8_t cap_len;    
    uint8_t cfg_type;   
    uint8_t bar;         // Which BAR to find it
    uint8_t padding[3]; 
    uint32_t offset;    
    uint32_t length;    
};

struct virtio_pci_common_cfg {
    uint32_t device_feature_select; /* read-write */
    uint32_t device_feature; /* read-only for driver */
    uint32_t driver_feature_select; /* read-write */
    uint32_t driver_feature; /* read-write */
    uint16_t msix_config; /* read-write */
    uint16_t num_queues; /* read-only for driver */
    uint8_t device_status; /* read-write */
    uint8_t config_generation; /* read-only for driver */
    
    /* About a specific virtqueue. */
    
    uint16_t queue_select; /* read-write */
    uint16_t queue_size; /* read-write */
    uint16_t queue_msix_vector; /* read-write */
    uint16_t queue_enable; /* read-write */
    uint16_t queue_notify_off; /* read-only for driver */
    uint64_t queue_desc; /* read-write */
    uint64_t queue_driver; /* read-write */
    uint64_t queue_device; /* read-write */
};

struct virtio_pci_notify_cap {
    struct virtio_pci_cap cap;
    uint32_t notify_off_multiplier; /* Multiplier for queue_notify_off. */
};

struct virtio_net_config {
    uint8_t mac[6];
    uint16_t status;                    // Little endian
    uint16_t max_virtqueue_pairs;       // little endian
    uint16_t mtu;                       // Exists if VIRTIO_NET_F_MTU is set, little endian
};

struct virtio_net_hdr {
    #define VIRTIO_NET_HDR_F_NEEDS_CSUM 1
    #define VIRTIO_NET_HDR_F_DATA_VALID 2
    #define VIRTIO_NET_HDR_F_RSC_INFO 4
    uint8_t flags;
    #define VIRTIO_NET_HDR_GSO_NONE 0
    #define VIRTIO_NET_HDR_GSO_TCPV4 1
    #define VIRTIO_NET_HDR_GSO_UDP 3
    #define VIRTIO_NET_HDR_GSO_TCPV6 4
    #define VIRTIO_NET_HDR_GSO_ECN 0x80
    uint8_t gso_type;
    uint16_t hdr_len;
    uint16_t gso_size;
    uint16_t csum_start;
    uint16_t csum_offset;
    uint16_t num_buffers;
};

struct queue_buffer {
    uint32_t addr;
    uint32_t length;
    uint16_t flags;
    uint16_t next;
};

/*
struct virtio_desc {
    uint32_t addr;
    uint32_t len;
    uint16_t flags;
    uint16_t next;
};

struct disk_avail {
    uint16_t flags;
    uint16_t idx;
    uint16_t ring[];
};
*/
struct virtio_used_item {
    uint32_t id;
    uint32_t len;
};

struct virtio_used {
    uint16_t flags;
    uint16_t index;
    struct virtio_used_item ring[];
};

struct virtq {
    struct queue_buffer *buffers;
    struct virtio_avail *avail;
    struct virtio_used *used;
};

void virtio_net_init(void);
void virtio_send_buffer(char *buffer, int size);
int virtionet_send_packet(uint32_t *payload, unsigned int size);
int virtio_net_recv();
void alloc_desc(void);

#endif
