#ifndef NET_H
#define NET_H


struct net_interface {
    struct net_interface *next;
    uint8_t family;
    struct net_dev *dev;
};

struct net_dev {
    struct net_dev *next;
    struct net_interface *ifs; 
    int index;
    uint16_t type;
    uint16_t mtu;
    uint16_t flags;
    uint16_t hlen;
    uint16_t alen;
    uint8_t addr[16];
    uint8_t peer[16];
    uint8_t broadcast[16];
    void *priv;
};


#endif
