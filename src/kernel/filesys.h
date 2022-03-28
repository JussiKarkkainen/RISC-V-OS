#ifndef FILESYS_H
#define FILESYS_H

#include <stdint.h>
#include "disk.h"
#include "locks.h"

#define LOGSIZE 30
#define MAXLOGOPS 10
#define NUMINODE 50
#define INODE_PER_BLOCK (BUFFER_SIZE / sizeof(struct disk_inode))
#define BITMAP_PER_BLOCK (BUFFER_SIZE * 8)
#define NDIRECT 12
#define NINDIRECT (BUFFER_SIZE / sizeof(unsigned int))
#define MAXFILE (NDIRECT + NINDIRECT)
#define DIRSIZE 14
#define DIR_TYPE 1
#define FILE_TYPE 2
#define DEV_TYPE 3
#define ROOTING 1
#define ROOTDEV 1       // Device number of root disk
#define MAXPATH 128

struct buffer {
    int valid;
    int busy;
    int changed;
    int disk;
    unsigned int dev;
    int blockno;
    struct sleeplock lock;
    unsigned int refcount;
    struct buffer *prev;
    struct buffer *next;
    char data[BUFFER_SIZE];
};

struct superblock {
    int magic;
    int size;          // Size of filesystem
    int num_block;     // Number of data blocks
    int num_log;       // log blocks
    int log_start;     // Block num of first log block
    int num_inodes; 
    int inode_start;
    int bitmap_start;
};

struct disk_inode {
    uint16_t type;              // Type of the file, 0 for free
    uint16_t major_dev_num;     // major device number
    uint16_t minor_dev_num;     // minor device number
    uint16_t num_link;          // Number of links to inode in filesystem
    unsigned int size;           // Size of the file
    unsigned int addresses[13];  // Data block addresses
};

#define T_DIR     1   
#define T_FILE    2   
#define T_DEVICE  3

struct stat {
    int dev;
    unsigned int inode_num;
    short type;
    short num_link;
    uint32_t size;
};

struct direntry {
    unsigned short inode_num;
    char name[DIRSIZE];
};

// Functions from bufcache.c
void buffer_init(void);
struct buffer *buffer_read(int dev, int blockno);
void buffer_write(struct buffer *buf);
void buffer_release(struct buffer *buf);
void dec_buf_refcount(struct buffer *buf);
void inc_buf_refcount(struct buffer *buf);

// Functions from filesys.c
//  logging layer
void init_log(int dev, struct superblock *sb);
void recover_from_log(void);
void begin_op(void);
void end_op(void);
void write_log(void);
void log_write(struct buffer *buf);
void write_header(void);
void read_header(void);
void commit(void);
void cpy_log_to_home(int recover);

//  buffer allocation
unsigned int buffer_alloc(unsigned int dev);
void buffer_free(unsigned int dev, unsigned int b);
void buffer_zero(int dev, int buffer_num);

//  inode layer
void init_inode(void);
struct inode *inode_alloc(int dev, uint16_t type);
struct inode *inode_get(unsigned int dev, int inode_num);
struct inode *inode_dup(struct inode *inode);
void inode_lock(struct inode *inode);
void inode_unlock(struct inode *inode);
void inode_put(struct inode *inode);
void inode_truncate(struct inode *inode);
void inode_update(struct inode *inode);
int read_inode(struct inode *inode, int user_dst, uint32_t dst, unsigned int off, unsigned int n);
int write_inode(struct inode *inode, int user_dst, uint32_t dst, unsigned int off, unsigned int n);
void copy_stat_inode(struct inode *inode, struct stat *stat);

//  directory layer
struct inode *dir_lookup(struct inode *inode, char *name, unsigned int *poff);
int dir_link(struct inode *inode, char *name, unsigned int inode_num);

// path names
struct inode *name_fetch(char *path, int nameiparent, char *name);
struct inode *name_inode(char *path);
struct inode *nameiparent(char *path, char *name);
char *skip_elem(char *path, char *name);

#endif
