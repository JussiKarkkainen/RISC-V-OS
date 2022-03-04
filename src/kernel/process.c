#include <stdint.h>
#include "process.h"


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
