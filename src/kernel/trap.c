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

static inline uint32_t get_stval(void) {
    uint32_t stval;
    asm volatile("csrr %0, stval" :: "=r" (stval));
    return stval;
}

static inline void write_stvec(uint32_t ktrapvec) {
    asm volatile("csrw stvec, %0" : : "r" (ktrapvec));
}

int handle_interrupt() {
    // Check if external/device interrupt
    uint32_t scause = get_scause();
    if ((scause & INTERRUPT_BIT) == 1 && (scause & EXT_INTERRUPT == 9)) {
        // Interrupt given by PLIC
        // Can be uart or virtio disk interrupt     
         
    }
  
    // Check if software/timer interrutp
    else if (scause == SOFT_INTERRUPT) {
        
    }
}

int timer_interrupt() {
}

void yield_process(void) {
}


void ktrap(void) {

    uint32_t sepc = get_sepc();
    uint32_t sstatus = get_sstatus();
    uint32_t scause = get_scause();
    uint32_t stval = get_stval():

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
        // Print out register info and panic
        kprintf("scause: %x\n, sstatus: %x\n, stval: %x\n", scause, sstatus, stval);
        panic("kernel interrupt");
    }
        

    if (timer_interrupt() == 1) {
        yield_process();
    }
}

void ktrapvec();


void init_ktrapvec(void) {
    write_stvec((uint32_t)ktrapvec);
}



