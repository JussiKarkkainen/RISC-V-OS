#include "tcp.h"
#include "ipv4.h"
#include "../kernel/locks.h"
#include "../kernel/crypto.h"
#include "net.h"
#include "queue_handler.h"
#include "socket.h"
#include "arpa/inet.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include <stdint.h>
#include <stddef.h>

// tcp_receive/send_packet are used to actually send tcp packets,
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
struct tcp_control_block tcp_cb_table[TCP_CB_TABLE_SIZE];

void tcp_init(void) {
    initlock(&tcplock, "tcplock");
}

int tcp_assign_desc(void) {

    struct tcp_control_block *cb;
    acquire_lock(&tcplock);
    int i;
    for (i = 0; i < TCP_CB_TABLE_SIZE; i++) {
        cb = &tcp_cb_table[i];
        if (!cb->used) {
            cb->used = 1;
            release_lock(&tcplock);
            return i;
        }
    }
    kprintf("no free tcp control blocks\n");
    release_lock(&tcplock);
    return -1;
}
/*
int pushto_txq(struct tcb_control_block *cb, struct tcp_header *hdr, int len) {
    
   struct tcp_txq_entry *txqe = (struct tcp_txq_entry *)kalloc();
   if (!txqe) {
       return -1;
    }  

    txqe->segment = (struct tcp_header *)kalloc();
    if (!txqe->segment) {
        kfree((uint32_t)txqe);
        return -1;
    }
    
    memcpy(txqe->segment, hdr, len);
    txqe->len = len;

    if (cb->txq.head == NULL) {
        cb->txq.head = txq;
    } else {
        cb->txq.tail->next = txq;
    }
    
    cb->txq.tail = txq;

    return 0;
}
*/
int tcp_connect(int desc, struct sockaddr *addr, int addrlen) {
    
    struct tcp_control_block *cb, *tmp;
    struct sockaddr_in *sin;
    int i, p;

    if (SOCKET_INVALID(desc)) {
        kprintf("Invalid desc in tcp_connect(): %d\n", desc);
        return -1;
    }
    
    if (addr->sa_family != AF_INET) {
        kprintf("invalid sockaddr->sa_family in tcp_connect(): %p Only ipv4 is supported\n", addr->sa_family);
        return -1;
    }
    
    sin = (struct sockaddr_in *)addr;

    acquire_lock(&tcplock);
    cb = &tcp_cb_table[desc]; 
    cb->net_iface = (struct net_interface *)&net_interface_table[0];

    if (!cb->used || cb->state != TCP_CB_STATE_CLOSED) {
        kprintf("tcp_connect: Invalid control block: cb->used %d cb->state %d\n", cb->used, cb->state);
        release_lock(&tcplock);
        return -1;
    }
    // Assign an ephemeral port as a source port
    if (!cb->port) {
        for (i = TCP_SRC_PORT_MIN; i <= TCP_SRC_PORT_MAX; i++) {
            for (p = 0; p < TCP_CB_TABLE_SIZE; p++) {
               tmp = &tcp_cb_table[p];
               if (tmp->used && (tmp->port == htons((uint16_t)i))) {
                   break;
                }
            }
            cb->port = htons((uint16_t)i);
            break;
        }
        if (!cb->port) {
            release_lock(&tcplock);
            return -1;
        }
    }
    cb->peer.ip_addr = sin->sin_addr;
    cb->peer.port = sin->sin_port;
    cb->receive.wnd = sizeof(cb->window);

    // Random number for iss
    //cb->iss = isn_gen(cb->net_iface->gateway_ip, cb->port, cb->peer.ip_addr, cb->peer.port);
    cb->iss = 0;
    tcp_send_packet(cb, cb->iss, 0, TCP_FLG_SYN, NULL, 0);
    
    return -1; 
    /*
    cb->send.next += 1;
    cb-state = TCP_CB_SYN_SENT;
    while (cb->state == TCP_CB_SYN_SENT) {
        sleep(&tcp_cb_table[desc], &tcplock);
    }
    release_lock(&tcplock);
    return 0;
    */
}

/*
int tcp_send(int desc, uint8_t *buf, int len) {
    
    struct tcp_control_block *cb;

    if (SOCKET_INVALID(desc)) {
        return -1;
    }
    acquire_lock(&tcplock);
    cb = &tcp_cb_table[desc];
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
    cb = &tcp_cb_table[desc];
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

// RFC793 pages 64-75
void tcp_handle_state(struct tcp_control_block *cb, 
                      struct tcp_header *hdr, int len) {
    
    uint32_t seq_num, ack;
    int hdr_len, payload_len;
    hdr_len = ((hdr->off >> 4) << 2);
    payload_len = len - hdr_len;
    
    
    switch (cb->state) {
        case TCP_CB_STATE_CLOSED:
            
            if (FLAG_IS_SET(hdr->flags, TCP_FLG_RST)) {
                return;
            }
            if (!FLAG_IS_SET(hdr->flags, TCP_FLG_ACK)) {
                seq_num = 0;
                ack = hdr->sequence_num + hdr->header_length;
            } else {
                seq_num = hdr->ack_num;
                ack = 0;
                return;

        case TCP_CB_STATE_LISTEN:
            
            if (FLAG_IS_SET(hdr->flags, TCP_FLG_RST)) {
                return;
            }
            if (FLAG_IS_SET(hdr->flags, TCP_FLG_ACK)) {
                seq_num = hdr->ack_num;
                return;
            if (FLAG_IS_SET(hdr->flags, TCP_FLG_SYN)) {
                seq_num = hdr->ack_num;
                ack = 0;
                tcp_send_packet(cb, seq_num, ack, TCP_FLG_RST, NULL, 0);
                return; 

        case TCP_SB_STATE_SYN_SENT:      
            
            if (FLAG_IS_SET(hdr->flags, TCP_FLG_ACK)) {
                if (ntohl(hdr->ack_num) <= cb->iss || ntohl(hdr->ack_num) > cb->send.next) {
                    if (FLAG_IS_SET(hdr->flags, TCP_FLG_RST)) {
                        return;
                    } else {
                        seq_num = hdr->ack_num;
                        ack = 0;
                        tcp_send_packet(cb, seq_num, ack, TCP_FLG_RST, NULL, 0);
                        return;
                    }

            if (FLAG_IS_SET(hdr->flags, TCP_FLG_RST)) {
                if (cb->send.next <= ntohl(hdr->ack_num) && tohl(hdr->ack_num) <= cb->send.next) {
                    kprintf("error: connection reset\n");
                }
                return;
            }
            
            // There's a step here I might still need to implement
            // Check the security and precedence

            if (FLAG_IS_SET(hdr->flags, TCP_FLG_SYN)) {
                cb->receive.next = ntohl(hdr->seq_num) + 1;
                cb->irs = htohl(hdr->seq_num);
                if (FLAG_IS_SET(hdr->flags, TCP_FLG_ACK)) {
                    cb->send.una = ntohl(hdr->ack_num);
                    if (cb->send.una < cb->iss) {
                        cb->state = TCP_CB_STATE_ESTABLISHED;
                        seq_num = cb->send.next;
                        ack = receive.next;
                        tcp_send_packet(cb, seq_num, ack, TCP_FLG_ACK, NULL, 0);
                    }
                    return;
                }
                seq_num = cb->iss;
                ack = cb->receive.next;
                tcp_send_packet(cb, seq_num, ack, TCP_FLG_ACK | TCP_FLG_SYN, NULL, 0);
            }
            return;
        
        default:
            break;

    // TO-DO implement check_seq() returns -1 if not valid
    if (check_seq(hdr->sequence_num) < 0) {
        if (!FLAG_IS_SET(hdr->flags, TCP_FLG_RST)) {
            seq_num = cb->send.next;
            ack = cb->receive.next;
            tcp_send_packet(cb, seq_num, ack, TCP_FLG_ACK, NULL, 0);
        }
        return;
    }
    if (FLAG_IS_SET(hdr->flags, TCP_FLG_RST)) {
        
        switch(cb->state) {
            case TCP_CB_STATE_SYN_RCVD:
                if (cb->parent) {
                    cb->state = TCP_CB_STATE_LISTEN;
                    break;
                } else {
                    kprintf("connection refused\n");
                    cb->state = TCP_CB_STATE_CLOSED;
                    break;
                }

            case TCP_CB_STATE_ESTABLISHED:
            case TCP_CB_STATE_FIN_WAIT1:
            case TCP_CB_STATE_FIN_WAIT2:
            case TCP_CB_STATE_CLOSE_WAIT:
                kprintf("connection reset\n");
                cb->state = TCP_CB_STATE_CLOSED;
                break;

            case TCP_CB_STATE_CLOSING:
            case TCP_CB_STATE_LAST_ACK:
            case TCP_CB_STATE_TIME_WAIT:
                cb->state = TCP_CB_STATE_CLOSED;
                break;

            default:
                break;
        }
        return;
    }
    
    if (FLAG_IS_SET(hdr->flags, TCP_FLG_SYN)) {
        kprintf("coneection reset\n");
        cb->state = TCP_CB_STATE_CLOSED;
        return;
    }
    if (!FLAG_IS_SET(hdr->flags, TCP_FLG_ACK)) {
        return;
    }
    
    switch (cb->state) {
        case TCP_CB_STATE_SYN_RCVD:
            if (!(hdr->ack_num <= cb->send.next && cb->send.una <= hdr->ack_num)) {
                seq_num = hdr->ack_num;
                ack = 0;
                tcp_send_packet(cb, seq_num, ack, TCP_FLG_RST, NULL, 0);
            }
            cb->state = TCP_CB_STATE_ESTABLISHED;

        case TCP_CB_STATE_ESTABLISHED:
            
        default:
            break;
    }

    if (FLAG_IS_SET(hdr->flags, TCP_FLG_URG)) {

        switch (cb->state) {
            case TCP_CB_STATE_ESTABLISHED:
            case TCP_CB_STATE_FIN_WAIT1:
            case TCP_CB_STATE_FIN_WAIT2:
                break;
            case TCP_CB_STATE_CLOSE_WAIT:
            case TCP_CB_STATE_CLOSING:
            case TCP_CB_STATE_LAST_ACK:
            case TCP_CB_STATE_TIME_WAIT:
                break;

            default:
                break;
        }
    }

    if (plen) {
        switch (cb->state) {
            case TCP_CB_STATE_ESTABLISHED:
            case TCP_CB_STATE_FIN_WAIT1:
            case TCP_CB_STATE_DIN_WAIT2:
                memcpy(cb->window + (sizeof(cb->window) - cb->rcv.wnd), (uint8_t *)hdr + hlen, plen);
                cb->rcv.nxt = ntohl(hdr->seq_num) + plen;
                cb->rcv.wnd -= plen; 
                seq_num = cb->sedn.next;
                ack = cb->receive.next;
                tcp_send_packet(cb, seq_num, ack, TCP_FLG_ACK, NULL, 0);
                break;
            default:
                break;
        }
    }     
    
    if (FLAG_IS_SET(hdr->flags, TCP_FLG_SYN)) {
        cb->receive.next++;
        tcp_send_packet(cb, cb->send.next, cb->receive.next, TCP_FLG_ACK, NULL, 0);
        
        switch (cb->state) {
            case TCP_CB_STATE_SYN_RCVD:
            case TCP_CB_STATE_ESTABLISHED:
                cb->state = TCP_CB_STATE_CLOSE_WAIT;
                wakeup(cb);
                break;
            case TCP_CB_STATE_FIN_WAIT1:
                cb->state = TCP_CB_STATE_FIN_WAIT2;
                break;
            case TCP_CB_STATE_FIN_WAIT2:
                cb->state = TCP_CB_STATE_TIME_WAIT;
                wakeup(cb);
                break;
            default:
                break;
        }
        return;
    }
    return;
}

void tcp_receive_packet(struct net_interface *netif, uint8_t *segment, 
                        uint32_t *src_addr, uint32_t *dst_addr, uint32_t len) {

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
        cb = tcp_cb_table[i];
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
    if (cb = tcp_cb_table[TCP_CB_TABLE_SIZE]) {    
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

*/
void tcp_send_packet(struct tcp_control_block *cb, uint32_t seq_num, 
                     uint32_t ack_num, uint8_t flags, uint8_t *buf, int len) {
    
    uint8_t segment[1500];
    //uint8_t pseudo[12];
    struct tcp_header *tcp_header;
    //struct tcp_pseudo_hdr *pseudo_hdr;
    uint32_t pseudo = 0;
    uint32_t self, peer;
     
    memset(&segment, 0, sizeof(segment));
    tcp_header = (struct tcp_header *)segment;
    tcp_header->src_port = cb->port;
    tcp_header->dst_port = cb->peer.port;
    tcp_header->sequence_num = htonl(seq_num);
    tcp_header->ack_num = htonl(ack_num);
    tcp_header->offset = (sizeof(struct tcp_header) >> 2) << 4;
    tcp_header->flags = flags; 
    tcp_header->window_size = htons(cb->receive.wnd);
    tcp_header->tcp_checksum = 0;
    tcp_header->urgent_pointer = 0;
    
    if (buf != 0 && len != 0) {
        memcpy(tcp_header + 1, buf, len);
    }
    
    self = ((struct net_interface *)cb->net_iface)->ip;
    
    peer = cb->peer.ip_addr.s_addr;
    pseudo += (self >> 16) & 0xffff;
    pseudo += self & 0xffff;
    pseudo += (peer >> 16) & 0xffff;
    pseudo += peer & 0xffff;
    pseudo += htons((uint16_t)PROTOCOL_TYPE_TCP);
    pseudo += htons(sizeof(struct tcp_header) + len);
    /*
    pseudo_hdr = (struct tcp_pseudo_hdr *)pseudo;
    memset(&pseudo, 0, sizeof(pseudo));
    pseudo_hdr->src_addr = cb->net_iface->ip;
    pseudo_hdr->dst_addr = cb->peer.ip_addr.s_addr;
    pseudo_hdr->reserved = 0;
    pseudo_hdr->protocol = PROTOCOL_TYPE_TCP;
    */
    uint16_t checksum =  ipv4_checksum((uint16_t *)tcp_header, (sizeof(struct tcp_header) + len), pseudo);
    kprintf("%p\n", checksum);
    return;
    tcp_header->tcp_checksum = ipv4_checksum((uint16_t *)tcp_header, (sizeof(struct tcp_header) + len), pseudo);
    
    
    // args = net_iface?, dst_ip_addr, data, len, flags, protocol
    /*
    ipv4_send_packet(cb->net_iface, peer, (uint8_t *)hdr, 
                     sizeof(struct tcp_hdr) + len, flags, IP_PROTOCOL_TCP);
  
    tcp_txq_add(cb, hdr, sizeof(struct tcp_hdr) + len);
    return len; 
*/
}

/*
uint32_t isn_gen(uint32_t localip, uint16_t localport, 
                 uint32_t remoteip, uint16_t remoteport) {

    //uint32_t m = get_time();
    
    uint8_t *in = localip + localport + remoteip + remoteport;
    int inlen = sizeof(*in);

    uint8_t *key = key_gen();
    uint8_t buf[8];
    
    siphash24(in, inlen, key, buf);
    uint32_t isn = (uint32_t)(*buf);
    return isn;
}
*/
