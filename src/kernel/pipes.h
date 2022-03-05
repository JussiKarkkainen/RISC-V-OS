#ifndef PIPES_H
#define PIPES_H

#define PIPESIZE 512

struct pipe {
    struct spinlock lock;
    char data[PIPESIZE];
    unsigned int num_read;  // Number of bytes read
    unsigned int num_write; // Number of bytes writte
    int read_open;      
    int write_open;
};

#endif
