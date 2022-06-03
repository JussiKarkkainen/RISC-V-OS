#include "virtio-net.h"
#include "pcie.h"
#include "../kernel/locks.h"
#include "../kernel/paging.h"
#include "../kernel/disk.h"
#include "../libc/include/stdio.h"
#include <stdint.h>


void virtio_net_init(void) {

}



void init_queue(int index) {

}



void virtio_send_buffer(char *buffer, int size) {
}


int virtio_net_send_packet(uint32_t *payload, unsigned int size) {
}

int virtio_net_recv(void) {
}


