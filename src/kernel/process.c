#include <stdint.h>
#include "process.h"
#include "regs.h"


void sleep(void *sleep_channel, struct spinlock *lock) {
    struct process *proc = get_process_struct();
    
    acquire_lock(&proc->lock);
    release_lock(lock);
    
    // Put process to sleep
    proc->sleep_channel = sleep_channel
    proc->state = SLEEPING;

    scheduler();

    proc->sleep_channel = 0;
    release_lock(&proc->lock);
    acquire_lock(lock);
}

// Wake up processes sleeping on sleep channel
void wakeup(void *sleep_channel) {
    struct process *proc;

    for (proc = process; proc < &process[MAXPROC]; proc++) {
        if (proc != get_process_struct()) {
            acquire_lock(proc->lock);
            if (proc->state == SLEEPING && proc->sleep_channel == sleep_channel) {
                proc->state = RUNNABLE;
            }
            release_lock(&proc->lock);
        }
    }
}

// This is the scheduler for CPUS. Its called from enter()
// and runs in an infinite loop. 
// 1. Choose process to run
// 2. call transfer() to start running that process
// 3. Transfer eventually returns control
void cpu_scheduler(void) {

    struct process *proc;
    struct cpu *cpu = get_cpu_struct();
    c->proc = 0;
    
    while (1) {
        enable_intr();

        for (proc = process; proc < process[MAXPROC]; proc++) {
            acquire_lock(&proc->lock);
            if (proc->state = RUNNABLE) {
                
                proc->state = RUNNING;
                transfer(&cpu->context, &proc-context);

                cpu->proc = 0;
            }
            release_lock(&proc->lock);
        }
    }
}


void scheduler(void) {
    int intr_prev_state;
    struct process *proc = get_process_struct();

    if (is_holding) {
        panic("scheduler, is_holding");
    }
    if (get_cpu_struct()->depth_lock_intr_disable != 1) {
        panic("scheduler, depth_lock_intr_disable");
    }
    if (p->state == RUNNING) {
        panic("scheduler, running process")
    }
    if (get_intr()) {
        panic("scheduler, interrupts");
    }

    intr_prev_state = get_cpu_struct->intr_prev_state;
    transfer(&proc->context, &get_cpu_struct()->context);
    get_cpu_struct()->depth_lock_intr_disable = depth_lock_intr_disable;
}

void yield_process(void) {
    struct process *proc = fet_process_struct();
    acquire_lock(&proc->lock);
    proc->state = RUNNABLE;
    scheduler();
    release(&proc->lock);
}

// tp register contains the current hartid
static inline uint32_t read_tp(void) {
    uint32_t tp;
    asm volatile("mv %0, tp" : "=r" (tp));
    return tp;
}

int which_cpu(void) {
    int cpu_id = read_tp();
    return cpu_id;
}


// Fetch current cpu struct
struct cpu *get_cpu_struct(void) {
    int hart_id = which_cpu();
    struct cpu *c = &cpus[id];
    return c;
}

// Fetch current process struct
struct process *get_process_struct(void) {
    lock_intr_disable();
    struct cpu *c = get_cpu_struct();
    struct process *p = c->proc;
    return p;
}
