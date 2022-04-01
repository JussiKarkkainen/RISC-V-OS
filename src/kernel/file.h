#ifndef FILE_H
#define FILE_H

#include <stdint.h>
#include "locks.h"

#define NUMFILE 100
#define NUMDEV 10
#define MAXOPBLOCKS 10

struct file {
    enum { FD_NONE, FD_PIPE, FD_INODE, FD_DEVICE } type;
    int ref;
    char readable;
    char writable;
    struct pipe *pipe;
    struct inode *inode;
    unsigned int offset;
    short major_dev_num;
};

struct inode {
    unsigned int dev;
    int inode_num;
    unsigned int refcount;
    struct sleeplock lock;
    int valid;                  // Inode has been read from disk 
    
    // Copy of disk inode
    short type;
    short major_dev_num;
    short minor_dev_num;
    short num_link;
    unsigned int size;
    unsigned int addresses[13]; 
};

struct devsw {
  int (*read)(int, uint32_t, int);
  int (*write)(int, uint32_t, int);
};

// Functions from file.c
void file_init(void);
struct file *file_alloc(void);
struct file *file_inc(struct file *file);
void file_close(struct file *file);
int file_stat(struct file *file, uint32_t address);
int read_file(struct file *file, uint32_t address, int n);
int write_file(struct file *file, uint32_t address, int n);
struct file *file_dup(struct file *f);

#endif
