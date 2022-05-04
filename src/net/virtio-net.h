#ifndef NET_H
#define NET_H

#define VIRT_PCIE 0x30000000


void virtio_net_init(void);

int virtio_net_send();
int virtio_net_recv();

#endif
