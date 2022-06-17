#include "tcp.h"
#include "ipv4.h"
#include "../kernel/locks.h"
#include "net.h"
#include "queue_handler.h"
#include "socket.h"
#include "arpa/inet.h"
#include <stdint.h>
#include <stddef.h>

// tcp_receive_packet are used to actually send tcp packets,
// while tcp_send, and tcp_recv are used as an interface between
// socket syscalls and tcp protocol

#define SOCKET_INVALID(x) (x < 0 || x >= TCP_CB_TABLE_SIZE)
#define TCP_CB_STATE_TX_ISREADY(x) (x->state == TCP_CB_STATE_ESTABLISHED || x->state == TCP_CB_STATE_CLOSE_WAIT)
#define TCP_CB_STATE_RX_ISREADY(x) (x->state == TCP_CB_STATE_ESTABLISHED || x->state == TCP_CB_STATE_FIN_WAIT1 || x->state == TCP_CB_STATE_FIN_WAIT2)


static struct spinlock tcplock;
struct tcp_control_block cb_table[TCP_CB_TABLE_SIZE];


void tcp_init(void) {
    initlock(&tcplock, "tcplock");
}


void tcp_assign_desc(void) {

    struct tcp_control_block *cb;
    acquire_lock(&tcplock);
    int i;

    for (i = 0; i < TCP_CB_TABLE_SIZE; i++) {
        cb = cb_table[i];
        if (!cb->used) {
            cb->used = 1;
            release_lock(&tcplock);
            return i;
        }
    kprintf("no free tcp control blocks\n");
    release_lock(&tcplock);
    return -1;
}


void tcp_connect(int desc, struct sockaddr *addr, int addrlen) {
    
    struct tcp_control_block *cb;
    
    if (SOCKT_INVALID(desc)) {
        return -1;
    }
    
    if (addr-sa_family != AF_INET) {
        return -1;
    }
    
    acquire_lock(&tcplock);
    cb = cb_table[desc]; 

}

void tcp_send(int desc, uint8_t *buf, int len) {
    
    struct tcp_control_block *cb;

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
    struct tcp_control_block *cb;
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


void tcp_handle_state(struct tcp_control_block *cb, struct tcp_header *hdr, int len) {
    
    switch (cb->state) {
        case TCP_CB_STATE_CLOSED:
        case TCP_CB_STATE_LISTEN:
        case TCP_SB_STATE_SYN_SENT:      
    }
}

void tcp_receive_packet(struct net_interface *netif, uint8_t *segment, uint32_t *src_addr, uint32_t dst_addr, uint32_t len) {
}


void tcp_send_packet(struct tcp_control_block *cb, uint32_t seq_num, uint32_t ack_num, uint8_t flags, uint8_t buf, int len) {
    uint8_t segment[1500];
    struct tcp_header *tcp_header;
    uint32_t pseudo;
    uint32_t self, peer;

    memset(&segment, 0, sizeof(segment));

    tcp_header = (struct tcp_header *)segment;
    tcp_header->src_port = cb->port;
    tcp_header->dst_port = cb->peer.port;
    tcp_header->sequence_num = htonl(seq_num);
    tcp_header->acknowledge_num = htonl(ack);
    tcp_header->offset = (sizeof(struct tcp_header) >> 2) << 4;
    tcp_header->flags = flags; 
    tcp_header->window_size = htons(cb.receive.wnd);
    tcp_header->tcp_checksum = 0;
    tcp_header->urgent_pointer = 0;
    
    memcpy(hdr + 1, buf, len);
    //self = ((struct netif_ip *)cb->net_iface)->unicast;
    peer = cb->peer.ip_addr;
    pseudo += (self >> 16) & 0xffff;
    pseudo += self & 0xffff;
    pseudo += (peer >> 16) & 0xffff;
    pseudo += peer & 0xffff;
    pseudo += htons((uint16_t)PROTOCOL_TYPE_TCP);
    pseudo += htons(sizeof(struct tcp_header) + len);
    tcp_header->sum = ipv4_checksum((uint16_t *)hdr, sizeof(struct tcp_hdr) + len, pseudo);
    ipv4_send_packet(cb->net_iface, &peer, (uint8_t *)hdr, sizeof(struct tcp_hdr) + len, flags, IP_PROTOCOL_TCP);
//    tcp_txq_add(cb, hdr, sizeof(struct tcp_hdr) + len);
    return len; 

}


