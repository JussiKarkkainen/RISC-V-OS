#ifndef FILE_H
#define FILE_H

#include <stdint.h>

#define NUMFILE 100

struct file {
    enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
    int ref;
    char readable;
    char writable;
    struct pipe *pipe;
    struct inode *inode;
    unsigned int offset;
    short major_dev_num;
}

struct inode {
    unsigned int dev;
    unsigned int inode_num;
    unsigned int refcnt;
    struct sleeplock slock;
    int valid;                  // Inode has been read from disk 
    
    // Copy of disk inode
    uint16_t type;
    uint16_t major;
    uint16_t minor;
    uint16_t num_link;
    unsigned int size;
    unsigned int addresses[13]; 
}

// Functions from file.c
void file_init(void);
struct file *file_alloc(void);
struct file *file_inc(struct file *file);
void file_close(struct file *file);
int file_stat(struct file *file, uint32_t address);

#endif
