#include "virtio-net.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../libc/include/stdio.h"
#include <stdint.h>

#define R(r) ((volatile uint32_t *)(VIRT_PCIE + (r))) 

struct Virtio_net {
        
    struct spinlock net_lock;

}__attribute__((aligned (PGESIZE))) net;


void virtio_net_init(void) {
    
    initlock(&net.net_lock, "net_lock");
   
    // 1. Reset device
    uint32_t *val = *R(0);
    kprintf("val: %p\n", *val);



}
