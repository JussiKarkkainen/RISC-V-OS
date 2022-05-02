#include <stdint.h>
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "virtio-net.hh"

class Virtio_net_driver {
    
    struct spinlock *net_lock;

}__attribute__((aligned (PGESIZE))) virtio_net;

void virtio_net_init(void) {
    
    initlock(&virtio_net.net_lock, "net_lock");


}
