// Setup cpu for supervisor mode

#include "process.h"
#include "trap.h"
#include "regs.h"
#include "../libc/include/stdio.h"
#include "paging.h"

#define MSTATUS_MIE (1L << 3)
#define SIE_SEIE (1L << 9)
#define SIE_STIE (1L << 5)
#define SIE_SSIE (1L << 1)
#define MIE_MTIE (1L << 7)

#define CLINT_MTIMECMP(hartid) (CLINT + 0x4000 + 8*(hartid))
#define CLINT_MTIME (CLINT + 0xBFF8)

void enter();
void timer_init();

extern void tvec();

__attribute__((aligned (16))) char stacks[4096 * MAXCPUS];

uint32_t scratch[MAXCPUS * 32];

void timer_init(void) {
    
    // Get the id of current hart
    uint32_t hart_id = get_mhartid();
    
    // Ask clint for timer interrupt, clint is memory-mapped to 0x2000000.
    uint32_t interval = 1000000;
    *(uint64_t*)CLINT_MTIMECMP(hart_id) = *(uint64_t*)CLINT_MTIME + interval;
    
    // prepare scratch register
    uint32_t *scratch_ptr = &scratch[32 * hart_id];
    scratch_ptr[4] = CLINT_MTIMECMP(hart_id);
    scratch_ptr[5] = interval;
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
    mstatus &= ~(3L << 11);
    mstatus |= (1L << 11);
    write_mstatus(mstatus);
    // Set mepc to point to enter(), so when we call mret, execution jumps there
    write_mepc((uint32_t)enter);

    // Disable paging
    write_satp(0);

    // All traps are handled in supervisor mode. This can be done by setting
    // writing to the medeleg and mideleg registers and the mie registers
    write_medeleg(0xffff);
    write_mideleg(0xffff);
    
    // Configure physical memory protection
    write_pmpaddr0(0xffffffff);
    write_pmpcfg0(0xf);
    
    uint32_t a  = get_sie();
    uint32_t b = a | 0x222L;
    asm volatile("csrw sie, %0" : : "r" (b)); 

    // enable clock interrupts
    timer_init();
    
    // Write hart_id to tp register
    uint32_t id = get_mhartid();
    write_tp(id);
    // Jump to enter()
    asm volatile("mret");

}
