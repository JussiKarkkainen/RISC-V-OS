#include "regs.h"

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
