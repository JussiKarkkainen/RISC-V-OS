#ifndef LOCKS_H
#define LOCKS_H

// Struct to describe whether struct is locked
struct spinlock {
    unsigned int locked;
    char *name;
    struct cpu *cpu;
};

struct sleeplock {
    unsigned int locked;
    struct spinlock lock;
    char *name;
    int process_id;
};

#endif
