#ifndef TCP_H
#define TCP_H

#include <stdint.h>
#include "net.h"

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

#define TCP_FLG_FIN 0x01
#define TCP_FLG_SYN 0x02
#define TCP_FLG_RST 0x04
#define TCP_FLG_PSH 0x08
#define TCP_FLG_ACK 0x10
#define TCP_FLG_URG 0x20

struct tcp_header {
    uint16_t src_port;
    uint16_t dst_port;
    uint32_t sequence_num;
    uint32_t acknowledge_num;
    uint8_t header_length : 4;
    uint8_t resv : 4;
    uint8_t flags;
    uint16_t window_size;
    uint16_t tcp_checksum;
    uint16_t urgent_pointer;
} __attribute__((packed));


struct tcp_txq_entry {
    struct tcp_hdr *segment;
    uint16_t len;
    struct tcp_txq_entry *next;
};

struct tcp_tx_queue_head {
    struct tcp_txq_entry *head;
    struct tcp_txq_entry *tail;
};

struct tcp_control_block {
    uint8_t used;
    uint8_t state;
    struct net_interface *net_iface;
    uint16_t port;
    struct {
        uint32_t ip_addr;
        uint16_t port;
    } peer;
    struct {
        uint32_t next;
        uint32_t una;
        uint16_t up;
        uint32_t wl1;
        uint32_t wl2;
        uint16_t wnd;
    } send;
    uint32_t iss;
    struct {
        uint32_t next;
        uint16_t up;
        uint16_t wnd;
    } receive;
    uint32_t irs;
    struct tcp_tx_queue_head txq;
    uint8_t window[PGESIZE];
    struct tcp_control_block *parent;
    struct queue_head backlog;
}

int tcp_init(void);
void tcp_connect(int desc, struct sockaddr *addr, int addrlen);
void tcp_assign_desc(void);
void tcp_send(int desc, uint8_t *buf, int len);
void tcp_recv(int desc, uint8_t addr, int n);
void tcp_send_packet(

#endif
