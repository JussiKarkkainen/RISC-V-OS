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
#define FLAG_IS_SET(x, y) ((x & 0x3f) == (y))

#define TCP_CB_STATE_TX_ISREADY(x) (x->state == TCP_CB_STATE_ESTABLISHED \
                                    || x->state == TCP_CB_STATE_CLOSE_WAIT)

#define TCP_CB_STATE_RX_ISREADY(x) (x->state == TCP_CB_STATE_ESTABLISHED \
                                    || x->state == TCP_CB_STATE_FIN_WAIT1 \
                                    || x->state == TCP_CB_STATE_FIN_WAIT2)


static struct spinlock tcplock;
struct tcp_control_block cb_table[TCP_CB_TABLE_SIZE];


void tcp_init(void) {
    initlock(&tcplock, "tcplock");
}


void tcp_assign_desc(void) {

    struct tcp_control_block *cb;
    acquire_lock(&tcplock);
    int i, p;

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


int tcp_connect(int desc, struct sockaddr *addr, int addrlen) {
    
    struct tcp_control_block *cb, *tmp;
    struct sockaddr_in *sin;
    int i;

    if (SOCKT_INVALID(desc)) {
        return -1;
    }
    
    if (addr-sa_family != AF_INET) {
        return -1;
    }
    
    sin = (struct sockaddr_in *)addr;

    acquire_lock(&tcplock);
    cb = cb_table[desc]; 
    
    if (!cb->used || cb->state != TCP_CB_STATE_CLOSED) {
        release_lock(&tcplock);
        return -1;
    }

    // Assign an ephemeral port as a source port
    if (!cb_port) {
        for (i = TCP_SRC_PORT_MIN; i <= TCP_SRC_PORT_MAX; i++) {
           for (p = 0; p < TCP_CB_TABLE_SIZE; p++) {
               tmp = cb_table[i];
               if (tmp->used && tmp->port == htons((uint16_t)i)) {
                   break;
                }
               cb->port = htons((uint16_t)i);
               break;
            }
        if (!cb->port) {
            release_lock(&tcplock);
            return -1;
        }
    }

    cb->peer.addr = sin->sin_addr;
    cb->peer->port = sin->sin_port;
    cb_receive->wnd = sizeof(cb->window);

    cb->iss = isn_gen();
    uint32_t seq_num = cb->iss;
    
    tcp_send_packet(cb, seq_num, TCP_FLAG_SYN, NULL, 0);
     
    cb->send.next += 1;
    cb-state = TCP_CB_SYN_SENT;
    while (cb->state == TCP_CB_SYN_SENT) {
        sleep(&cb_table[desc], &tcplock);
    }
    release_lock(&tcplock);
    return 0;
}


int tcp_send(int desc, uint8_t *buf, int len) {
    
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
    tcp_send_packet(cb, cb->send.next, cb->receive.next, 
                    TCP_FLG_ACK | TCP_FLG_PSH, buf, len);
    cb->send.next += len;
    release_lock(&tcplock);
    return 0;
}


int tcp_recv(int desc, uint8_t addr, int n) {
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


void tcp_handle_state(struct tcp_control_block *cb, 
                      struct tcp_header *hdr, int len) {
    
    switch (cb->state) {
        case TCP_CB_STATE_CLOSED:
        case TCP_CB_STATE_LISTEN:
        case TCP_SB_STATE_SYN_SENT:      
    }
}

void tcp_receive_packet(struct net_interface *netif, uint8_t *segment, 
                        uint32_t *src_addr, uint32_t dst_addr, uint32_t len) {

    struct tcp_header *tcp_hdr;
    struct tcp_control_block *cb, *tcb;
    uint32_t pseudo = 0;
    int i;

    if (len < sizeof(struct tcp_header)) {
        return;
    }

    tcp_header = (struct tcp_header *)segment;
    pseudo += *src >> 16;
    pseudo += *src & 0xffff;
    pseudo += *dst >> 16;
    pseudo += *dst & 0xffff;
    pseudo += htons((uint16_t)PROTOCOL_TYPE_TCP);
    pseudo += htons(len);
    
    if (ipv4_checksum((uint16_t *)tcp_header, len, pseudo) != 0) {
        kprintf("tcp receive checksum error\n");
        return;
    }

    acquire_lock(&tcplock);
    for (i = 0; i < TCP_CB_TABLE_SIZE; i++) {
        cb = cb_table[i];
        if (!cb->used) {
            if (!tcb) {
                tcb = cb;
            }
        else if ((!cb->net_iface || cb->net_iface == iface) && cb->port == tcp_hdr->dst) {
            if (cb->peer.addr == *src && cb->peer.port == hdr->src) {
                break;
            }
            if (cb->state == TCP_CB_STATE_LISTEN && !lcb) {
                lcb = cb;
            }
        }

    }
    if (cb = cb_table[TCP_CB_TABLE_SIZE]) {    
        if (!tcb || !lcb || !FLAG_IS_SET(tcp_hdr, TCP_FLG_SIN)) {
            release_lock(&tcplock);
            return;
        }
        cb = tcb;
        cb->used = 1;
        cb->state = tcb->state;
        cb->net_iface = iface;
        cb->port = lcb->port;
        cb->peer.addr = *src;
        cb->peer.port = tcp_hdr->src_port;
        cb->receive.wnd = sizeof(cb->window);
        cb->parent = lcb;
    }

    tcp_handle_state(cb, tcp_hdr, len);
    release_lock(&tcplock);
    return;
}


void tcp_send_packet(struct tcp_control_block *cb, uint32_t seq_num, 
                     uint32_t ack_num, uint8_t flags, uint8_t buf, int len) {
    
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
    tcp_header->sum = ipv4_checksum((uint16_t *)hdr, 
                                     sizeof(struct tcp_hdr) + len, pseudo);
    
    ipv4_send_packet(cb->net_iface, &peer, (uint8_t *)hdr, 
                     sizeof(struct tcp_hdr) + len, flags, IP_PROTOCOL_TCP);
//  
//  tcp_txq_add(cb, hdr, sizeof(struct tcp_hdr) + len);
    return len; 

}

uint32_t isn_gen(uint32_t localip, uint16_t localport, 
                 uint32_t remoteip, uint16_t remoteport) {

    uint32_t m = get_time();
    
    uint8_t *in = localip + localport + remoteip + remoteport;
    int inlen = sizeof(*in);

    key = siphash_key_gen();
    uint8_t buf[8];
    
    siphash24(in, inlen, key, buf);
    uint32_t isn = (uint32_t)(*buf + m);
    return isn;
}
