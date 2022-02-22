// Setup cpu for supervisor mode

#include "kernel.c"

#define SIE_SEIE (1 << 9)
#define SIE_STIE (1 << 5)
#define SIE_SSIE (1 << 1)

void enter();

static inline uint32_t get_mstatus(void) {
    uint32_t mstatus;
    asm volatile("csrr %0, mstatus" : : "=r" (mstatus));
    return mstatus;
}

static inline void write_mepc(uint32_t enter) {
    asm volatile("csrw mepc, %0" : : "r" (enter));
}

static inline void write_satp(uint32_t x) {
    asm volatile("csrw satp, %0" : : "r" (x));
}

static inline void write_medeleg(uint32_t x) {
    asm volatile("csrw medeleg, %0" : : "r" (x));
}

static inline void write_mideleg(uint32_t x) {
    asm volatile("csrw mideleg, %0" : : "r" (x));
}

static inline void write_sie(uint32_t x) {
    asm volatile("csrw mie, %0" : : "r" (x));
}

static inline uint32_t get_sie(void) {
    uint32_t sie;
    asm volatile("csrr %0, sie" : : "=r" (sie));
    return sie;
}

static inline void write_pmpaddr0(uint32_t x) {
    asm volatile("csrw pmpaddr0, %0" : : "r" (x));
}

static inline void write_sie(uint32_t x) {
    asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}

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

