#include "tcp.h"
#include "ipv4.h"
#include "../kernel/locks.h"
#include "socket.h"
#include <stdint.h>
#include <stddef.h>

// tcp_receive_packet are used to actually send tcp packets,
// while tcp_send, and tcp_recv are used as an interface between
// socket syscalls and tcp protocol

#define TCP_CB_STATE_CLOSED      0
#define TCP_CB_STATE_LISTEN      1
#define TCP_CB_STATE_SYN_SENT    2
#define TCP_CB_STATE_SYN_RCVD    3
#define TCP_CB_STATE_ESTABLISHED 4
#define TCP_CB_STATE_FIN_WAIT1   5
#define TCP_CB_STATE_FIN_WAIT2   6
#define TCP_CB_STATE_CLOSING     7
#define TCP_CB_STATE_TIME_WAIT   8
#define TCP_CB_STATE_CLOSE_WAIT  9
#define TCP_CB_STATE_LAST_ACK    10

#define TCP_CB_TABLE_SIZE 16

#define SOCKET_INVALID(x) (x < 0 || x >= TCP_CB_TABLE_SIZE)
#define TCP_CB_STATE_TX_ISREADY(x) (x->state == TCP_CB_STATE_ESTABLISHED || x->state == TCP_CB_STATE_CLOSE_WAIT)
#define TCP_CB_STATE_RX_ISREADY(x) (x->state == TCP_CB_STATE_ESTABLISHED || x->state == TCP_CB_STATE_FIN_WAIT1 || x->state == TCP_CB_STATE_FIN_WAIT2)

#define TCP_FLG_FIN 0x01
#define TCP_FLG_SYN 0x02
#define TCP_FLG_RST 0x04
#define TCP_FLG_PSH 0x08
#define TCP_FLG_ACK 0x10
#define TCP_FLG_URG 0x20

static struct spinlock tcplock;
struct tcp_cb cb_table[TCP_CB_TABLE_SIZE];

void tcp_init(void) {
    initlock(&tcplock, "tcplock");
}


void tcp_connect(int desc, struct sockaddr *addr, int addrlen) {
}

void tcp_assign_desc(void) {

    struct tcp_cb *cb;
    acquire_lock(&tcplock);
    
    for (int i = 0; i < TCP_CB_TABLE_SIZE; i++) {
        cb = cb_table[i];
        if (!cb->used) {
            cb->used = 1;
            release_lock(&tcplock);
            return i;
        }
    release_lock(&tcplock);
    return -1;
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
    struct tcp_cb *cb;
    int total, len;

    if (SOCKET_INVALID(desc)) {
        return -1;
    }
    acquire_lock(&tcplock);
    cb = &cb_table[desc];
    if (!cb->used) {
        release_lock(&tcplock);
        return -1;
    }
    while (!(total = sizeof(cb->window) - cb->receive.wnd)) {
        if (!TCP_CB_STATE_RX_ISREADY(cb)) {
            release(&tcplock);
            return 0;
        }
        sleep(cb, &tcplock);
    }
    len = size < total ? size : total;
    memcpy(buf, cb->window, len);
    memmove(cb->window, cb->window + len, total - len);
    cb->receive.wnd += len;
    release_lock(&tcplock);
    
    return len;
}




void tcp_receive_packet(struct *ipv4hdr, uint8_t *data) {
}


void tcp_send_packet(uint8_t *segment, size_t len, ip_addr_t *src, ip_addr_t *dst, struct net_interface *net_iface) {

    ipv4_send_packet();
}




