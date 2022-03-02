#include "regs.h"



void acquire_lock(struct spinlock *lock) {
    lock_intr_disable();

    // Check if current cpu is holding the lock
    int current_holdr = (lock->locked && lk->cpu = get_cpu_struct());
    if (current_holdr) {
        panic("acquire_lock");
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
    
    // 
    lk->cpu = get_cpu_struct();
}



// Functions for matched interrupt enabling ad disabling
void lock_intr_enable(void) {
    struct cpu *c = get_cpu_struct();
    if (get_intr()) {
        panic("lock_intr_enable");
    }
    if (c->depth_lock_intr_disable < 1) {
        panic("lock_intr_enablei, intr depth");
    }
    c->depth_lock_intr_disable -= 1;
    if ((c->depth_lock_intr_disable == 0) && (c->depth_lock_intr_disable)) {
        enable_intr();
    }
}

void lock_intr_disable(void) {
    int intr_state = get_intr();
    disable_intr();

    if (get_cpu_struct()->depth_lock_intr_disable == 0) {
        get_cpu_struct->intr_prev_state = old;
    }
    get_cpu_struct()->depth_lock_intr_disable += 1;
}
