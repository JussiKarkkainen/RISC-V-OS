#include "virtio-net.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../libc/include/stdio.h"
#include <stdint.h>


struct Virtio_net {
        
    struct spinlock net_lock;

}__attribute__((aligned (PGESIZE))) net;


void virtio_net_init(void) {
    
    initlock(&net.net_lock, "net_lock");



}


int virtio_net_send() {
}


int virtio_net_recv() {
}
