#ifndef FILESYS_H
#define FILESYS_H

struct buffer {
    int valid;
    int busy
    int changed;
    int disk;
    unsigned int dev;
    unsigned int blockno;
    struct sleeplock lock;
    unsigned int refcount;
    struct buffer *prev;
    struct buffer *next;
    char data[BUFFER_SIZE];
};

// Functions from bufcache.c
void buffer_init(void);
struct buffer *buffer_read(int dev, int blockno);
void buffer_write(struct buffer *buf);
void buffer_release(struct buffer *buf);

#endif
