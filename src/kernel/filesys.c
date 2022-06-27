#include "filesys.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include "process.h"
#include <stddef.h>
#include "file.h"

struct superblock sb;

void read_superblock(int dev, struct superblock *sb) {
    struct buffer *buf;
    buf = buffer_read(dev, 1);
    memmove(sb, buf->data, sizeof(*sb));
    buffer_release(buf);
}

void filesys_init(int dev) {
    read_superblock(dev, &sb);
    if (sb.magic != FSMAGIC) {
        panic("invalid filesys");
    }
    init_log(dev, &sb);
}

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
    
    if (sizeof(struct log_header) >= BUFFER_SIZE) {
        panic("logheader too big during init_log");
    }
    initlock(&log.lock, "log");
    log.start = sb->log_start;
    log.size = sb->size;
    log.dev = dev;
    recover_from_log();
}

void recover_from_log(void) {

    read_header();
    cpy_log_to_home(1);
    log.loghead.count = 0;
    write_header();

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
            release_lock(&log.lock);
            break;
        }
    }
}

// Call at the end of syscall and decrease log.num_syscall
// If the last op, commit
void end_op(void) {
    
    int make_commit = 0;

    acquire_lock(&log.lock);
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
    
    release_lock(&log.lock);

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

void log_write(struct buffer *buf) {
    
    int i;

    acquire_lock(&log.lock);
    if (log.loghead.count >= LOGSIZE || log.loghead.count >= log.size - 1) {
        panic("transaction too big, log_write");
    }
    if (log.num_syscalls < 1) {
        panic("log_write outside of transaction");
    }

    for (i = 0; i < log.loghead.count; i++) {
        if (log.loghead.block[i] == buf->blockno) {
            break;
        }
    }
    log.loghead.block[i] = buf->blockno;
    if (i == log.loghead.count) {
        inc_buf_refcount(buf);
        log.loghead.count++;
    }
    release_lock(&log.lock);
}


// Read log header from disk into memory log header
void read_header(void) {
    struct buffer *buf = buffer_read(log.dev, log.start);
    struct log_header *log_head = (struct log_header *)(buf->data);
    
    for (int i = 0; i < log.loghead.count; i++) {
        log.loghead.block[i] = log_head->block[i];
    }
    
    buffer_release(buf);
}


void write_header(void) {
    
    struct buffer *buf = buffer_read(log.dev, log.start);
    struct log_header *lockhead = (struct log_header *)buf->data;
    lockhead->count = log.loghead.count;
    
    for (int i = 0; i < log.loghead.count; i++) {
        lockhead->block[i] = log.loghead.block[i];
    }
    buffer_write(buf);
    buffer_release(buf);
}

// Similar in implemetation to write_log()
void cpy_log_to_home(int recover) {
    
    for (int i = 0; i < log.loghead.count; i++) {
        struct buffer *logbuf = buffer_read(log.dev, log.start+i+1);
        struct buffer *dest = buffer_read(log.dev, log.loghead.block[i]);
        memmove(dest->data, logbuf->data, BUFFER_SIZE);
        buffer_write(dest);

        if (recover == 0) {
            dec_buf_refcount(dest);
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
        log.loghead.count = 0;
        write_header();
    }
}

unsigned int buffer_alloc(unsigned int dev) {
    
    int i, j, k;
    struct buffer *buf;
    
    buf = 0;
    for(i = 0; i < sb.size; i += BITMAP_PER_BLOCK) {
        buf = buffer_read(dev, (i / BITMAP_PER_BLOCK + sb.bitmap_start));
        for(j = 0; j < BITMAP_PER_BLOCK && i + j < sb.size; j++) {
            k = 1 << (i % 8);
            if((buf->data[j/8] & k) == 0) {
                buf->data[j/8] |= k;  
                log_write(buf);
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
    memset(buf->data, 0, BUFFER_SIZE);
    log_write(buf);
    buffer_release(buf);
}

void buffer_free(unsigned int dev, unsigned int b) {
    
    struct buffer *buf;
    int i, j;

    buf = buffer_read(dev, (b / BITMAP_PER_BLOCK + sb.bitmap_start));
    i = b % BITMAP_PER_BLOCK;
    j = 1 << (i % 8);

    if ((buf->data[i/8] & j) == 0) {
        panic("block alredy free");
    }
    buf->data[i/8] &= ~j;
    log_write(buf);
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
        initsleeplock(&inode_table.inode[i].lock, "inode sleeplocks");
    }
}

// When the OS makes a new file, it calls inode_alloc()
// Mark it allocated by changing the structs type
struct inode *inode_alloc(int dev, uint16_t type) {

    struct buffer *buf;
    struct disk_inode *dinode;
    int inode_num;
    for (inode_num = 1; inode_num < sb.num_inodes; inode_num++) {
        buf = buffer_read(dev, (inode_num / INODE_PER_BLOCK + sb.inode_start));
        dinode = (struct disk_inode *)buf->data + inode_num % INODE_PER_BLOCK;
        
        if (dinode->type == 0) {
            memset(dinode, 0, sizeof(*dinode));
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
struct inode *inode_get(unsigned int dev, int inode_num) {
    
        struct inode *inode, *empty;

        acquire_lock(&inode_table.lock);

        empty = 0;
        
        // Check if inode is in inode table
        for (inode = &inode_table.inode[0]; inode < &inode_table.inode[NUMINODE]; inode++) {
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
        inode->inode_num = inode_num;
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

    acquire_sleeplock(&inode->lock);

    if(inode->valid == 0) {

        buf = buffer_read(inode->dev, inode->inode_num / INODE_PER_BLOCK + sb.inode_start);
        dinode = (struct disk_inode*)buf->data + inode->inode_num % INODE_PER_BLOCK;
        inode->type = dinode->type;
        inode->major_dev_num = dinode->major_dev_num;
        inode->minor_dev_num = dinode->minor_dev_num;
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

    release_sleeplock(&inode->lock);
}


// INODE CONTENT

void inode_put(struct inode *inode) {

    acquire_lock(&inode_table.lock);

    if (inode->refcount == 1 && inode->valid && inode->num_link == 0) {

        acquire_sleeplock(&inode->lock);

        release_lock(&inode_table.lock);

        inode_truncate(inode);
        inode->type = 0;
        inode_update(inode);
        inode->valid = 0;

        release_sleeplock(&inode->lock);
        acquire_lock(&inode_table.lock);
    }
    inode->refcount--;
    release_lock(&inode_table.lock);
}

void inode_truncate(struct inode *inode) {

    int i, j;
    struct buffer *buf;
    unsigned int *a;

    for (i = 0; i < NDIRECT; i++) {
        if (inode->addresses[i]) {
            buffer_free(inode->dev, inode->addresses[i]);
            inode->addresses[i] = 0;
        }
    }

    if (inode->addresses[NDIRECT]) {
        buf = buffer_read(inode->dev, inode->addresses[NDIRECT]);
        a = (unsigned int *)buf->data;
        for (j = 0; j < NDIRECT; j++) {
            if (a[j]) {
                buffer_free(inode->dev, a[j]);
            }
        }
        buffer_release(buf);
        buffer_free(inode->dev, inode->addresses[NDIRECT]);
        inode->addresses[NDIRECT] = 0;
    }
    
    inode->size = 0;
    inode_update(inode);
}

// Copy a modified in-memory inode to disk.
// Caller must hold lock
// Needs to be called when any part in inode-> struct changes
void inode_update(struct inode *inode) {

   struct buffer *buf;
   struct disk_inode *dinode;

   buf = buffer_read(inode->dev, (inode->inode_num / INODE_PER_BLOCK + sb.inode_start));
   dinode = (struct disk_inode *)buf->data + inode->inode_num % INODE_PER_BLOCK;
   dinode->type = inode->type;
   dinode->major_dev_num = inode->major_dev_num;
   dinode->minor_dev_num = inode->minor_dev_num;
   dinode->num_link = inode->num_link;
   dinode->size = inode->size;

   memmove(dinode->addresses, inode->addresses, sizeof(inode->addresses));
   log_write(buf);
   buffer_release(buf);
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

    if (buffer_num < NINDIRECT) {
        if ((addr = inode->addresses[NDIRECT]) == 0) {
            inode->addresses[NDIRECT] = addr = buffer_alloc(inode->dev);
        }
        buf = buffer_read(inode->dev, addr);
        a = (unsigned int *)buf->data;
        if ((addr = a[buffer_num]) == 0) {
            a[buffer_num] = addr = buffer_alloc(inode->dev);
            log_write(buf);
        }
        buffer_release(buf);
        return addr;
  }

  panic("buffer_map: out of range");
}

struct inode *inode_dup(struct inode *inode) {
    acquire_lock(&inode_table.lock);
    inode->refcount++;
    release_lock(&inode_table.lock);
    return inode;
}

// Read data from inode
int read_inode(struct inode *inode, int user_dst, uint32_t dst, unsigned int off, unsigned int n) {
    
    unsigned int i, j;
    struct buffer *buf;

    if (off > inode->size || off + n < off) {
        return 0;
    }
    if (off + n > inode->size) {
        n = inode->size - off;
    }

    for (i = 0; i < n; i += j, off += j, dst += j) {
        buf = buffer_read(inode->dev, buffer_map(inode, off / BUFFER_SIZE));
        j = (n - i) < (BUFFER_SIZE - off % BUFFER_SIZE) ? (n - i) : (BUFFER_SIZE - off % BUFFER_SIZE);
        if (either_copyout(user_dst, dst, buf->data + (off % BUFFER_SIZE), j) == -1) {
            buffer_release(buf);
            i = -1;
            break;
        }
        buffer_release(buf);
    }
    return n;
}

// Write data to inode, return number of bytes written
int write_inode(struct inode *inode, int user_src, uint32_t src, unsigned int off, unsigned int n) {

    unsigned int i, j;
    struct buffer *buf;
    
    if (off > inode->size || off + n < off) {
        return -1;
    }
    if (off + n > MAXFILE * BUFFER_SIZE) {
        return -1;
    } 

    for (i = 0; i < n; i += j, off += j, src += j) {
        buf = buffer_read(inode->dev, buffer_map(inode, off/BUFFER_SIZE));
        j = (n - i) < (BUFFER_SIZE - off % BUFFER_SIZE) ? (n - i) : (BUFFER_SIZE - off % BUFFER_SIZE);
        if (either_copyin(buf->data + (off % BUFFER_SIZE), user_src, src, j) == -1) {
           buffer_release(buf);
           break;
        } 
        log_write(buf);
        buffer_release(buf);
    }

    if (off > inode->size) {
        inode->size = off;
    }

    inode_update(inode);
    return i;
}

void copy_stat_inode(struct inode *inode, struct stat *stat) {
    stat->dev = inode->dev;
    stat->inode_num = inode->inode_num;
    stat->num_link = inode->num_link;
    stat->type = inode->type;
    stat->size = inode->size;
}


//  DIRECTORY LAYER

struct inode *dir_lookup(struct inode *inode, char *name, unsigned int *poff) {

    unsigned int offset, inode_num;
    struct direntry de;

    if (inode->type != DIR_TYPE) {
        panic("dir lookup, not dir");
    }
    
    for (offset = 0; offset < inode->size; offset += sizeof(de)) {
        if (read_inode(inode, 0, (uint32_t)&de, offset, sizeof(de)) != sizeof(de)) {
            panic("dir_lookup read_inode");
        }
        if (de.inode_num == 0) {
            continue;
        }
        if (strncmp(name, de.name, DIRSIZE) == 0) {
            if (poff) {
                *poff = offset;
            }
            inode_num = de.inode_num;
            return inode_get(inode->dev, inode_num);
        }
    }
    return 0;
}

int dir_link(struct inode *inode, char *name, unsigned int inode_num) {

    unsigned int offset;
    struct direntry de;
    struct inode *inod;

    if ((inod = dir_lookup(inode, name, 0)) != 0) {
        inode_put(inod);
    }

    for (offset = 0; offset < inode->size; offset += sizeof(de)) {
        if (read_inode(inode, 0, (uint32_t)&de, offset, sizeof(de)) != sizeof(de)) {
            panic("dir_link, read_inode");
        }
        if (de.inode_num == 0) {
            break;
        }
    }

    strcpy(de.name, name, DIRSIZE);

    de.inode_num = inode_num;
    if (write_inode(inode, 0, (uint32_t)&de, offset, sizeof(de)) != sizeof(de)) {
        panic("dir_link, write_inode");
    }

    return 0;
}


// PATH NAMES

// Look up and return the inode for a pathname
struct inode *name_fetch(char *path, int nameiparent, char *name) {

    struct inode *inode, *next;
    
    if (*path == '/') {
        inode = inode_get(ROOTDEV, ROOTING);
    }
    else {
        inode = inode_dup(get_process_struct()->cwd);
    }

    while ((path = skip_elem(path, name)) != 0) {
        inode_lock(inode);
        if (inode->type != DIR_TYPE) {
            inode_unlock(inode);
            inode_put(inode);
            return 0;
        }
        if (nameiparent && *path == '\0') {
            inode_unlock(inode);
            return inode;
        }
        if ((next = dir_lookup(inode, name, 0)) == 0) {
            inode_unlock(inode);
            inode_put(inode);
            return 0;
        }
        inode_unlock(inode);
        inode_put(inode);
        inode = next;
    }
    if (nameiparent) {
        inode_put(inode);
        return 0;
    }
    return inode;
}

char *skip_elem(char *path, char *name) {
    
    char *s;
    int len;

    while (*path == '/') {
        path++;
    }
    if (*path == 0) {
        return 0;
    }
    s = path;
    
    while (*path != '/' && *path != 0) {
        path++;
    }
    len = path - s;
    if (len >= DIRSIZE) {
        memmove(name, s, DIRSIZE);
    }
    else {
        memmove(name, s, len);
        name[len] = 0;
    }
    while(*path == '/') {
        path++;
    }
    return path;
}

struct inode *name_inode(char *path) {
    char name[DIRSIZE];
    return name_fetch(path, 0, name);
}

struct inode *nameiparent(char *path, char *name) {
    return name_fetch(path, 1, name);
}


