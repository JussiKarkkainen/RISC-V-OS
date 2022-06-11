#include "tcp.h"
#include "ipv4.h"
#include "../kernel/locks.h"
#include <stdint.h>
#include <stddef.h>

static struct spinlock tcplock;


void tcp_connect() {
}





void tcp_receive_packet(struct *ipv4hdr, uint8_t *data) {
}


void tcp_send_packet() {
}








int tcp_init(void) {
    initlock(&tcplock, "tcp_lock");
    return 0;
}



