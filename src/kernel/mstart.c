// Setup cpu for supervisor mode

#include "kernel.c"
#include "trap.h"

#define SIE_SEIE (1 << 9)
#define SIE_STIE (1 << 5)
#define SIE_SSIE (1 << 1)

void enter();

void mstart(void) {
    // Clear the mstatus MPP bits and set them to supervisor mode
    uint32_t mstatus = get_mstatus();
    mstatus &= ~(3 << 11);
    msstatus |= (1 << 11);

    // Set mepc to point to enter(), so when we call mret, execution jumps there
    write_mepc((uint32_t)enter);

    // Disable paging
    write_satp(0);

    // All traps are handled in supervisor mode. This can be done by setting
    // writing to the medeleg and mideleg registers and the mie registers
    write_medeleg(0xffff);
    write_mideleg(0xffff);
    uint32_t sie = get_sie();
    write_sie((((sie | SIE_SSIE) | SIE_STIE) | SIE_SEIE));
    
    // Configure physical memory protection
    write_pmpaddr0(0xffffffff);
    write_pmpcfg0(0xf);

    // enable clock interrupts
    time_init();

    // Jump to enter()
    asm volatile("mret");

}

