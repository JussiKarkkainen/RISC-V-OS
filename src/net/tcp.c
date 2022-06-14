#include "tcp.h"
#include "ipv4.h"
#include "../kernel/locks.h"
#include "socket.h"
#include <stdint.h>
#include <stddef.h>

// tcp_receive_packet are used to actually send tcp packets,
// while tcp_send, and tcp_recv are used as an interface between
// socket syscalls and tcp protocol

#define TCP_CB_STATE_ESTABLISHED 4
#define TCP_CB_STATE_CLOSE_WAIT  9

#define TCP_CB_TABLE_SIZE 16
#define SOCKET_INVALID(x) (x < 0 || x >= TCP_CB_TABLE_SIZE)
#define TCP_CB_STATE_TX_ISREADY(x) (x->state == TCP_CB_STATE_ESTABLISHED || x->state == TCP_CB_STATE_CLOSE_WAIT)

#define TCP_FLG_FIN 0x01
#define TCP_FLG_SYN 0x02
#define TCP_FLG_RST 0x04
#define TCP_FLG_PSH 0x08
#define TCP_FLG_ACK 0x10
#define TCP_FLG_URG 0x20

static struct spinlock tcplock;


int tcp_init(void) {
    initlock(&tcplock, "tcplock");
    return 0;
}


void tcp_connect(int desc, struct sockaddr *addr, int addrlen) {
}

void tcp_open(void) {
}

void tcp_send(int desc, uint8_t *buf, int len) {
    
    struct tcp_cb *cb;

    if (SOCKET_INVALID(desc)) {
        return -1;
    }
    acquire_lock(&tcplock);
    cb = &cb_table[desc];
    if (!cb->used) {
        release_lock(&tcp_lock);
        return -1;
    }
    if (!CB_STATE_IS_READY(cb)) {
        release_lock(&tcp_lock);
        return -1;
    }
    tcp_send_packet(cb, cb->send.next, cb->receive.next, TCP_FLG_ACK | TCP_FLG_PSH, buf, len);
    cb->send.next += len;
    release_lock(&tcplock);
    return 0;
}


void tcp_recv(int desc, uint8_t addr, int n) {
}




void tcp_receive_packet(struct *ipv4hdr, uint8_t *data) {
}


void tcp_send_packet() {

    ipv4_send_packet();
}




