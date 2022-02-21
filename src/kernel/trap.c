#include "trap.h"
#include "../libc/stdio/panic.c"

static inline uint32_t get_sepc(void) {
    uint32_t sepc;
    asm volatile("csrr %0, sepc" : : "=r" (sepc));
    return sepc;
}

static inline uint32_t get_sstatus(void) {
    uint32_t sstatus;
    asm volatile("csrr %0, sstatus" : : "=r" (sstatus));
    return sstatus;
}

static inline uint32_t get_scause(void) {
    uint32_t scause;
    asm volatile("csrr %0, scause" : : "=r" (scause));
    return scause;
}



int handle_interrupt() {
    // Check if external/device interrupt





    // Check if software/timer interrutp
    

}

int timer_interrupt() {
}

void yield_process(void) {
}


void ktrap(void) {

    uint32_t sepc = get_sepc();
    uint32_t sstatus = get_sstatus();
    uint32_t scause = get_scause();

    // Make sure interrupt comes from supervisor mode
    if ((sstatus & SSTATUS_SPP) == 0) {
        panic("trap not in supervisor mode");
    }
    // Make sure interrupts are not enabled
    if ((sstatus & SSTATUS_SIE) == 1) {
        panic("interrupts are enabled");
    }

    // trap can be either device interrupt or exceptions
    // handle_interrupt deals with device interrupt. If trap is
    // an external interrupt, we call panic() and stop executing
    if (handle_interrupt() == 0) {
        panic("kernel interrupt");
    }
        

    if (timer_interrupt() == 1) {
        yield_process();
    }
}

