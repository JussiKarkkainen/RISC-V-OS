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



// Raad from disk
struct buffer *buffer_read(void) {
}

// Write to disk
void buffer_write(void) {
}



