// Buffer-Cache layer of the fileystem
// The job of the buffer cache lyer is to synchronize access to disk blocks and
// cache recently used buffers in a buffer cache. Main interface goes through 
// buffer_read and buffer_write, the former reading a buffer from disk to memory
// and the latter writing a modified buffer back to the disk.


#include "disk.h"
#include "locks.h"
#include "process.h"

struct {
    
    struct spinlock lock;

    // Create NUMBUF amount of buffers for buffer cache to hold
    struct buffer[NUMBUF];
    
    // Buffers are in a linked list connected by buffer structs
    // prev and next. Can be referred to with list_head.next/prev. 
    struct buffer list_head;
} buffer_cache;


// Initialize the doubly linked list of buffers
void buffer_init(void) {

    struct buffer *buf;

    // Initialize lock
    initlock(&buffer_cache.lock, "buffer_cache lock");

    // Create a linked list of buffers
    buffer_cache.head.prev = &buffer_cache.head;
    buffer_cache.head.next = &buffer_cache.head;
    
    // loop over buffers in cache
    for (buf = buffer_cache.buffer; buf < buffer_cache.buffer+NUMBUF; buf++) {
        b->next = buffer_cache.list_head.next;
        b->prev = &buffer_cache.list_head;
        initsleep(&buf->lock, "buffer_lock");
        buffer_cache.list_head.next->prev = buf;
        buffer_cachw.list_head.next = buf;
    }
}


// Read from disk
struct buffer *buffer_read(void) {
}

// Write to disk
void buffer_write(struct buffer *buf) {
}



