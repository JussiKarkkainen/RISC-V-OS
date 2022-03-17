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

struct superblock {
    unsigned int magic;
    unsigned int size;          // Size of filesystem
    unsigned int num_block;     // Number of data blocks
    unsigned int num_log;       // log blocks
    unsigned int log_start;     // Block num of first log block
    unsigned int num_inodes; 
    unsigned int inode_start;
    unsigned int bitmap_start;
};

// Functions from bufcache.c
void buffer_init(void);
struct buffer *buffer_read(int dev, int blockno);
void buffer_write(struct buffer *buf);
void buffer_release(struct buffer *buf);

#endif
