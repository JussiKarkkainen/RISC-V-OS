#ifndef FILESYS_H
#define FILESYS_H

#include <stdint.h>

#define LOGSIZE 30
#define MAXLOGOPS 10

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

struct disk_inode {
    uint16_t type;              // Type of the file, 0 for free
    uint16_t major_dev_num;     // major device number
    uint16_t minor_dev_num;     // minor device number
    uint16_t num_link;          // Number of links to inode in filesystem
    unsigned int size           // Size of the file
    unsigned int adresses[13];  // Data block addresses

// Functions from bufcache.c
void buffer_init(void);
struct buffer *buffer_read(int dev, int blockno);
void buffer_write(struct buffer *buf);
void buffer_release(struct buffer *buf);
void dec_refcount(struct buffer *buf);

// Functions from filesys.c
//  logging layer
void init_log(int dev, struct superblock *sb);
void recover_from_log(void);
void begin_op(void);
void end_op(void);
void write_log(void);
void write_header(void);
void read_header(void);
void commit(void);
void cpy_log_to_home(int recover);

//  inode layer
void init_inode(void);
#endif
