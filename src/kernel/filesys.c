#include "filesys.h"
#include "disk.h"
#include "../libc/include/stdio.h"
#include <stddef.h>

struct superblock *sb;

// LOGGING
// The logging layer provides security in case of a crash by keeping a log of disk writes.
// 

struct log_header {
    // Used to indicate if there are transactions in the log, if yes holds how many blocks it contains
    int count;             
    // Array of sector numbers, one for each logged block
    int block[LOGSIZE];
};

struct log {
    struct spinlock lock;
    int start;
    int commit;             // Is committing?, if so, wait
    int dev;                
    int size;
    int num_syscalls;
    struct log_header loghead;
};

struct log log;

// Make sure logheader isn't too big
// Initialize lock
// Set log start, size, dev
// Call recover_from_log
void init_log(int dev, struct superblock *sb) {
    
    if (sizeof(struct logheader) >= BUFFER_SIZE) {
        panic("logheader too big during init_log");
    }
    initlock(&log.lock, "log");
    log.start = sb->log_start;
    log.size = sb->size;
    log.dev = dev;
    recover_from_log();
}

void recover_from_log(void) {
}

// Make sure that logging system is not committing and system calls
// writes don't exceed the unreserved log space. If they do, sleep
// Otherwise increment log.num_syscalls
void begin_op(void) {
    
    acquire_lock(&log.lock);
    while (1) {
        if (log.commit) {
            sleep(&log, &log.lock);
        }
        else if (log.loghead.count + (log.num_syscalls + 1) * MAXLOGOPS > LOGSIZE) {
            sleep(&log, &log.lock);
        }
        else {
            log.num_syscalls += 1;
            release(&log.lock);
            break;
        }
    }
}

// Call at the end of syscall and decrease log.num_syscall
// If the last op, commit
void end_op(void) {
    
    int make_connect = 0;

    acquire_loc(&log.lock);
    log.num_syscalls -= 1;

    if (log.commit) {
        panic("end_op log is committing");
    }
    if (log.num_syscalls == 0) {
        make_commit = 1;
        log.commit  = 1;
    }
    else {
        wakeup(&log);
    }

    if (make_commit) {
        commit();
        acquire_lock(&log.lock);
        log.commit = 0;
        wakeup(&log);
        release_lock(&log.lock);
    }
}

// loop over blocks in transaction (log header count) and copy blocks
// from cache to log
void write_log(void) {
    
    for (int i = 0; i < log.loghead.count; i++) {
        struct buffer *to = buffer_read(log.dev, log.start+i+1);
        struct buffer *from = buffer_read(log.dev, log.loghead.block[i]);
        memmove(to->data, from->data, BUFFER_SIZE);
        buffer_write(to);
        buffer_release(to);
        buffer_release(from);
    }
}

// Read log header from disk into memory log header
void read_header(void) {
    struct buffer *buf = buffer_read(log.dev, log.start);
    struct log_header lockhead = (struct log_header *)buf->data;
    
    for (int i = 0; i < log.loghead.count; i++) {
        log.loghead.block[i] = lockhead->block[i];
    }
    
    buffer_release(buf);
}


void write_header(void) {
    
    struct buffer *buf = buffer_read(log.dev, log.start);
    struct log_header *lockhead = (struct log_header *)buf->data;
    lockhead->count = log.logheader.count;
    
    for (int i = 0; i < log.loghead.count; i++) {
        lockhead->block[i] = log.logheader.block[i];
    }
    buffer_write(buf);
    buffer_release(buf);
}

// Similar in implemetation to write_log()
void cpy_log_to_home(int recover) {
    
    for (int i = 0; i < log.loghead.count; i++) {
        struct buffer *logbuf = buffer_read(log.dev, log.start+i+1);
        struct buffer *dest = buffer_read(log.dev, log.loghead.block[i]);
        memmove(logbuf, dest, BUFFER_SIZE);
        buffer_write(dest);

        if (recover == 0) {
            dec_refcount(dest);
        }
        buffer_release(logbuf);
        buffer_release(dest);
    }
}

// Write modified blocks from cache to log
// Write header to disk
// Copy log to home location
// Set no transaction to log header
// Erase transaction from log by calling write_header() again
void commit(void) {

    if (log.loghead.count > 0) {
        write_log();
        write_header();
        cpy_log_to_home(0);
        write_header();
}

unsigned int buffer_alloc(unsigned int dev) {
    
    int i, j, k;
    struct buffer *buf;
    
    buf = 0;
    for(i = 0; i < sb.size; i += BITMAP_PER_BLOCK) {
        buf = buffer_read(dev, (i / BITMAP_PER_BLOCK + sb.bitmap_start));
        for(j = 0; j < BPB && i + j < sb.size; j++) {
            k = 1 << (i % 8);
            if((buf->data[j/8] & k) == 0) {
                buf->data[j/8] |= k;  
                log_write(buf)_
                buffer_release(buf);
                buffer_zero(dev, i + j);
                return i + j;
            }
        }
    buffer_release(buf);
    
    }
    panic("buffer_alloc: no blocks left");
}

void buffer_zero(int dev, int buffer_num) {
    struct buffer *buf;

    buf = buffer_read(dev, buffer_num);
    memset(buffer->data, 0, BUFFER_SIZE;
    write_log(buf);
    buffer_release(buf);
}

void buffer_free(unsigned int dev, unsigned int b) {
    
    struct buffer *buf;
    int i, j;

    buf = buffer_read(dev, (b / BITMAP_PER_BLOCK + sb.bitmap_start));
    i = b % BITMAP_PER_BLOCK;
    j = 1 << (i % 8);

    if ((buf->data[i/8] & j) == 0) {
        panic("block alredy free");
    }
    buf->data[i/8] &= ~j:
    write_log(buf);
    buffer_release(buf);
}

// INODES
// An inode describes a single file. The inode struct holds metadata about the file
// such as the files type, size, list of blocks holding its content and number of links
// referring to the file. The inode are laid out on disk starting from inode_start
// as defined in the superblock. Each inode is tarcked with a number indicating its
// position.


// Struct to describe the inode table
struct {
    struct spinlock lock;
    struct inode inode[NUMINODE];
}inode_table;

void inode_init(void) {
    
    initlock(&inode_table.lock, "inode_table lock");
    for (int i = 0; i < NUMINODE; i++) {
        initsleeplock(&inode_table.inode.slock, "inode sleeplocks");
    }
}

// When the OS makes a new file, it calls inode_alloc()
// Mark it allocated by changing the structs type
struct inode *inode_alloc(unsigned int dev, uint16_t type) {

    struct buffer *buf;
    struct disk_inode *dinode;
    int inode_num;

    for (inode_num = 0; inode_num < sb.num_inodes; inode_num++) {
        buf = buffer_read(dev, );
        dinode = (struct disk_inode *)buf->data + inode_num % INODE_PER_BLOCK;
        
        if (dinode == 0) {
            memset(dinode, 0, sizeof(dinode));
            dinode->type = type;
            log_write(buf);
            buffer_release(buf);
            return inode_get(dev, inode_num);
        }
        buffer_release(buf);
    }
    panic("no inodes found");
}

// Finds the inode on disk with inode_num, and returns the in memory copy of that inode
struct inode *inode_get(unsigned int dev, unsigned int inode_num) {
    
        struct inode *inode, empty;

        acquire_lock(&inode_table.lock);

        empty = 0;
        
        // Check if inode is in inode table
        for (inode = &inode_table.inode[0]; inode < inode_table.inode[NUMINODE]; inode++) {
            if (inode->refcount > 0 && inode->dev == dev && inode->inode_num == inode_num) {
                inode->refcount++;
                release_lock(&inode_table.lock);
                return inode;
            }
            if (empty == 0 && inode->refcount == 0) {
                empty = inode;
            }
        }

        if (empty == 0) {
            panic("no inodes inode_get()");
        }
        
        inode = empty;
        inode->dev = dev;
        inode->inode_num = inode_num
        inode->refcount = 1;
        inode->valid = 0;
        release_lock(&inode_table.lock);
        return inode;
}

void inode_lock(struct inode *inode) {
    struct buffer *buf;
    struct disk_inode *dinode;

    if(inode == 0 || inode->refcount < 1)
        panic("up == 0 || inode->refcount < 1, inode_lock");

    acquire_sleep(&inode->lock);

    if(ip->valid == 0) {

        buf = buffer_read(inode->dev, inode_num / INODE_PER_BLOCK + sb->inode_start);
        dinode = (struct dinode*)buf->data + inode->inode_num % INODE_PER_BLOCK;
        inode->type = dinode->type;
        inode->major = dinode->major;
        inode->minor = dinode->minor;
        inode->num_link = dinode->num_link;
        inode->size = dinode->size;
        
        memmove(inode->addresses, dinode->addresses, sizeof(inode->addresses));
        buffer_release(buf);
        inode->valid = 1;
        
        if(inode->type == 0) {
            panic("no type in inode_lock");
        }
    }
}

void inode_unlock(struct inode *inode) {

    if (inode == 0 || !is_holding_sleeplock(&inode->lock) || inode->refcount < 1) {
        panic("inode_unlock");
    }

    release_sleep(&inode->lock);
}


unsigned int buffer_map(struct inode *inode, unsigned int buffer_num) {
  
    unsigned int addr, *a;
    struct buffer *buf;

    if (buffer_num < NDIRECT) {
        if ((addr = inode->addresses[buffer_num]) == 0) {
            inode->addresses[buffer_num] = addr = buffer_alloc(inode->dev);
        }
        return addr;
    }
    buffer_num -= NDIRECT;

    if (bn < NINDIRECT) {
        if ((addr = inode->addresses[NDIRECT]) == 0) {
            inode->addresses[NDIRECT] = addr = buffer_alloc(inode->dev);
        }
        buf = buffer_read(inode->dev, addr);
        a = (unsigned int*)buf->data;
        if ((addr = a[buffer_num]) == 0) {
            a[buffer_num] = addr = buffer_alloc(inode->dev);
            write_log(buf);
        }
        buffer_release(buffer_num);
        return addr;
  }

  panic("buffer_map: out of range");
}

//  DIIRECTORY LAYER





// PATH NAMES
