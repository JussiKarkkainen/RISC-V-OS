#include "tcp.h"
#include "ipv4.h"
#include "../kernel/locks.h"
#include "socket.h"
#include <stdint.h>
#include <stddef.h>

// tcp_receive_packet are used to actually send tcp packets,
// while tcp_send, and tcp_recv are used as an interface between
// socket syscalls and tcp protocol

static struct spinlock tcplock;


int tcp_init(void) {
    initlock(&tcplock, "tcplock");
    return 0;
}


void tcp_connect(int desc, struct sockaddr *addr, int addrlen) {
}

void tcp_open(void) {
}

void tcp_send(int desc, uint8_t *addr, int n) {
}


void tcp_recv(int desc, uint8_t addr, int n) {
}




void tcp_receive_packet(struct *ipv4hdr, uint8_t *data) {
}


void tcp_send_packet() {
}




