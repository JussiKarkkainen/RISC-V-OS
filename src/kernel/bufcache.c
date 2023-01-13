// Buffer-Cache layer of the fileystem
// The job of the buffer cache lyer is to synchronize access to disk blocks and
// cache recently used buffers in a buffer cache. Main interface goes through 
// buffer_read and buffer_write, the former reading a buffer from disk to memory
// and the latter writing a modified buffer back to the disk.

#include "disk.h"
#include "filesys.h"
#include "locks.h"
#include "process.h"
#include "../libc/include/stdio.h"

struct {
    
    struct spinlock lock;

    // Create NUMBUF amount of buffers for buffer cache to hold
    struct buffer buffer[NUMBUF];
    
    // Buffers are in a linked list connected by buffer structs
    // prev and next. Can be referred to with list_head.next/prev. 
    struct buffer list_head;
} buffer_cache;


// Initialize the doubly linked list of buffers
void buffer_init(void) {

    struct buffer *buf;

    // Initialize lock
    initlock(&buffer_cache.lock, "buffer_cache");

    // Create a linked list of buffers
    buffer_cache.list_head.prev = &buffer_cache.list_head;
    buffer_cache.list_head.next = &buffer_cache.list_head;
    
    // loop over buffers in cache
    for (buf = buffer_cache.buffer; buf < buffer_cache.buffer+NUMBUF; buf++) {
        buf->next = buffer_cache.list_head.next;
        buf->prev = &buffer_cache.list_head;
        initsleeplock(&buf->lock, "buffer_lock");
        buffer_cache.list_head.next->prev = buf;
        buffer_cache.list_head.next = buf;
    }
}


// Read from disk
struct buffer *buffer_get(unsigned int dev, unsigned int blockno) {
    
    struct buffer *buf;
    acquire_lock(&buffer_cache.lock);
    
    // Check if buffer is cached
    for (buf = buffer_cache.list_head.next; buf != &buffer_cache.list_head; buf = buf->next) {
        if ((buf->dev == dev) && (buf->blockno == blockno)) {
            buf->refcount++;
            release_lock(&buffer_cache.lock);
            acquire_sleeplock(&buf->lock);
            return buf;
        }
    }
    
    // Buffer isn't cached, recycle least recently used buffer
    for (buf = buffer_cache.list_head.prev; buf != &buffer_cache.list_head; buf = buf->prev) {
        if (buf->refcount == 0) {
            buf->valid = 0;
            buf->dev = dev;
            buf->blockno = blockno;
            buf->refcount = 1;
            release_lock(&buffer_cache.lock);
            acquire_sleeplock(&buf->lock);
            return buf;
        }
    }
    panic("buffer_get, no buffers");
    return buf;
}

struct buffer *buffer_read(unsigned int dev, unsigned int blockno) {
    struct buffer *buf;

    buf = buffer_get(dev, blockno);
    if (!buf->valid) {
        disk_read_write(buf, 0);
        buf->valid = 1;
    }
    return buf;
}


// Write to disk
void buffer_write(struct buffer *buf) {
    if (!is_holding_sleeplock(&buf->lock)) {
        panic("buffer_write");
    }
    disk_read_write(buf, 1);
}

// Buffer returned by buffer_read is still holding a lock, release it.jjkkk
void buffer_release(struct buffer *buf) {
    
    if (!is_holding_sleeplock(&buf->lock)) {
        panic("buffer_release, not holding lock");
    }
    
    release_sleeplock(&buf->lock);

    acquire_lock(&buffer_cache.lock);
    buf->refcount--;
    if (buf->refcount == 0) {
        buf->next->prev = buf->prev;
        buf->prev->next = buf->next;
        buf->next = buffer_cache.list_head.next;
        buf->prev = &buffer_cache.list_head;
        buffer_cache.list_head.next->prev = buf;
        buffer_cache.list_head.next = buf;
    }

    release_lock(&buffer_cache.lock);
}

void inc_buf_refcount(struct buffer *buf) {
    acquire_lock(&buffer_cache.lock);
    buf->refcount++;
    release_lock(&buffer_cache.lock);
}

void dec_buf_refcount(struct buffer *buf) {
    acquire_lock(&buffer_cache.lock);
    buf->refcount--;
    release_lock(&buffer_cache.lock);
}
