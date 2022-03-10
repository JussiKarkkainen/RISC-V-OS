// Setup cpu for supervisor mode

#include "process.h"
#include "trap.h"
#include "regs.h"
#include "../libc/include/stdio.h"
#include "paging.h"

#define MSTATUS_MIE (1 << 3)
#define SIE_SEIE (1 << 9)
#define SIE_STIE (1 << 5)
#define SIE_SSIE (1 << 1)
#define MIE_MTIE (1 << 7)

void enter();

extern void tvec();

__attribute__((aligned (16))) char stacks[4096 * MAXCPUS];

uint32_t scratch[MAXCPUS][5];

void timer_init(void) {
    // Get the id of current hart
    uint32_t hart_id = get_mhartid();
    // Ask clint for timer interrupt, clint is memory-mapped to 0x2000000.
    int interval = 1000000;
    *(uint32_t*)(CLINT + CLINT_OFFSET + (8 * hart_id)) = *(uint32_t*)((CLINT + 0xBFF8) + interval);


    // prepare scratch register
    uint32_t *scratch_ptr = &scratch[hart_id][0];
    scratch_ptr[3] = (0x2000000 + 0x4000 + (8 * hart_id));
    scratch_ptr[4] = interval;
    write_mscratch((uint32_t)scratch_ptr);


    // Set machine mode trap handler
    write_mtvec((uint32_t)tvec);

    // Enable machine mode interrupts
    write_mstatus(get_mstatus() | MSTATUS_MIE);

    // Enable machine mode timer interrupts
    write_mie(get_mie() | MIE_MTIE);
}

void mstart(void) {
    // Clear the mstatus MPP bits and set them to supervisor mode
    uint32_t mstatus = get_mstatus();
    mstatus &= ~(3 << 11);
    mstatus |= (1 << 11);
    write_mstatus(mstatus);

    // Set mepc to point to enter(), so when we call mret, execution jumps there
    write_mepc((uint32_t)enter);

    // Disable paging
    write_satp(0);

    // All traps are handled in supervisor mode. This can be done by setting
    // writing to the medeleg and mideleg registers and the mie registers
    write_medeleg(0xffff);
    write_mideleg(0xffff);
    uint32_t sie = get_sie();
    write_sie(sie | SIE_SEIE | SIE_STIE | SIE_SSIE);
    

    // Configure physical memory protection
    write_pmpaddr0(0xffffffff);
    write_pmpcfg0(0xf);

    // enable clock interrupts
    timer_init();
    
    // Write hart_id to tp register
    uint32_t id = get_mhartid();
    write_tp(id);
    // Jump to enter()
    asm volatile("mret");

}
