#include <stdint.h>
#include "process.h"

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
