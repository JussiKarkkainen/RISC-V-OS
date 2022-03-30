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

int pipealloc(struct file **f0, struct file **f1);
int pipewrite(struct pipe *p, int n, uint32_t addr);
int piperead(struct pipe *p, int n, uint32_t addr);

#endif
