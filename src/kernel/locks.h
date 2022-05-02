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
    struct spinlock spinlock;
    char *name;
    int process_id;
};

void lock_intr_enable(void);
void lock_intr_disable(void);
int is_holding(struct spinlock *lock);
void acquire_sleeplock(struct sleeplock *lock);
void release_sleeplock(struct sleeplock *lock);
void release_lock(struct spinlock *lock);
void acquire_lock(struct spinlock *lock);
void initlock(struct spinlock*, char*);
void initsleeplock(struct sleeplock *lock, char *name);
int is_holding_sleeplock(struct sleeplock *lock);

#endif
