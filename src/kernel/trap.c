// Machine mode trap handler

void ktrap(void) {

    uint32_t sepc = get_sepc;
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
        

    if (timer_interrupt == 1) {
        // deal with timer interrupt
    }
}

