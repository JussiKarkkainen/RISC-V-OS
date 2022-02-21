// Machine mode trap handler

#include "trap.h"


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
}

int timer_interrupt() {
}

void ktrap(void) {

    uint32_t sepc = get_sepc();
    uint32_t sstatus = get_sstatus();
    uint32_t scause = get_scause();

    // Make sure interrupt comes from supervisor mode
    if ((sstatus & SSTATUS_SPP) == 0) {
        kprintf("trap not in supervisor mode");
    }
    // Make sure interrupts are not enabled
    if (check_interrupts() == 0) {
        kprintf("interrupts are enabled");
    }
    if (handle_interrupt() == 0) {
        kprintf("kernel interrupt");
    }
        

    if (timer_interrupt() == 1) {
        // deal with timer interrupt
    }
}

