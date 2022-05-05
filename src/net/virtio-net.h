#ifndef NET_H
#define NET_H

#include <stdint.h>


void virtio_net_init(void);

int virtio_net_send();
int virtio_net_recv();

#endif
