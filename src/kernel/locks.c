#include "regs.h"
#include "locks.h"
#include "../libc/include/stdio.h"
#include "process.h"

void initlock(struct spinlock *lock, char *name) {
    lock->locked = 0;
    lock->name = name;
    lock->cpu = 0;
}

void initsleeplock(struct sleeplock *lock, char *name) {
    initlock(&lock->spinlock, "sleeplock");
    lock->name = name;
    lock->locked = 0;
    lock->process_id = 0;
}

void acquire_lock(struct spinlock *lock) {
    lock_intr_disable();

    // Check if current cpu is holding the lock
    if (is_holding(lock)) {
        panic("acquire_lock, is_holding, locks.c");
    }
    
    // When cpu needs to acquire a lock, we need to check if some other
    // cpu already has it. This spins until the lock is no longer used
    // by other cpus, after which it acquires the lock for current cpu. 
    // __sync_lock_test_and_set() is used because this
    // check needs to be atomic, so special hardware in the processor
    // is invoked to perform it.
    while(__sync_lock_test_and_set(&lock->locked, 1) != 0) {
        ;
    }

    // Tell compiler and preprocessor to ensure, that the critical section's 
    // memory references happen strictly after the lock is acquired.
    __sync_synchronize();
      
    lock->cpu = get_cpu_struct();
}

void release_lock(struct spinlock *lock) {
    // If current cpu doesn't have the lock, panic
    int current_holdr = (lock->locked && (lock->cpu = get_cpu_struct()));
    if (!current_holdr) {
        kprintf("lock %s\n", lock->name);
        panic("release_lock, not current holder");
    }
    
    // Tell compiler and preprocessor to ensure, that the critical section's 
    // memory references happen strictly after the lock is acquired.
    __sync_synchronize();

    // Same as lock->locked = 0, except its atomic 
    __sync_lock_release(&lock->locked);

    lock_intr_disable();
}

int is_holding(struct spinlock *lock) {
    int r;
    r = (lock->locked && lock->cpu == get_cpu_struct());
    return r;
}

void acquire_sleeplock(struct sleeplock *lock) {
    
    acquire_lock(&lock->spinlock);
    while (lock->locked) {
        sleep(lock, &lock->spinlock);
    }
    
    lock->locked = 1;
    lock->process_id = get_process_struct()->process_id;
    release_lock(&lock->spinlock);
}

void release_sleeplock(struct sleeplock *lock) {
    acquire_lock(&lock->spinlock);
    lock->locked = 0;
    lock->process_id = 0;
    wakeup(lock);
    release_lock(&lock->spinlock);
}

int is_holding_sleeplock(struct sleeplock *lock) {
    
    acquire_lock(&lock->spinlock);
    int i = ((lock->locked) && (lock->process_id == get_process_struct()->process_id));
    release_lock(&lock->spinlock);
    
    return i;
}

// Functions for matched interrupt enabling and disabling
void lock_intr_enable(void) {
    struct cpu *c = get_cpu_struct();
    if (get_intr()) {
        panic("lock_intr_enable");
    }
    if (c->depth_lock_intr_disable < 1) {
        panic("lock_intr_enablei, intr depth");
    }
    c->depth_lock_intr_disable -= 1;
    if ((c->depth_lock_intr_disable == 0) && (c->intr_prev_state)) {
        enable_intr();
    }
}

void lock_intr_disable(void) {
    int intr_state = get_intr();
    disable_intr();

    if (get_cpu_struct()->depth_lock_intr_disable == 0) {
        get_cpu_struct()->intr_prev_state = intr_state;
    }
    get_cpu_struct()->depth_lock_intr_disable += 1;
}
