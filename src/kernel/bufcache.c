// Buffer-Cache layer of the fileystem

#include "disk.h"




struct {
    
    struct spinlock lock;

    // Create NUMBUF amount of buffers for buffer cache to hold
    struct buffer[NUMBUF];
    
    // Buffers are in a linked list connected by buffer structs
    // prev and next. Can be referred to with list_head.next/prev. 
    struct buffer list_head;
} buffer_cache;








