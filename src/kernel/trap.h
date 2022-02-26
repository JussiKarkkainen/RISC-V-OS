#ifndef TRAP_H
#define TRAP_H

#define SSTATUS_SPP (1 << 8)
#define SSTATUS_SIE (1 << 1)
#define INTERRUPT_BIT (1 << 32)
#define EXT_INTERRUPT 0xff
#define SOFTWARE_INTR 0x80000001
#define CLEAR_SIP_SSIP ~2

struct trapframe {
    uint32_t kernel_pagetable;
    uint32_t kernel_sp;
    uint32_t kernel_trap;
    uint32_t saved_pc;
    uint32_t hartid;
    uint32_t ra;
    uint32_t sp;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t s0;
    uint32_t s1;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
};

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

static inline void write_pmpcfg0(uint32_t x) {
    asm volatile("csrw pmpcfg0, %0" : : "r" (x));
}

static inline uint32_t get_mhartid(void) {
    uint32_t mhartid;
    asm volatile("csrr %0, mhartid" : : "=r" (mhartid));
    return mhartid;


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

static inline void write_stvec(uint32_t x) {
    asm volatile("csrw stvec, %0" : : "r" (x));
}

static inline void write_mscratch(uint32_t x) {
    asm volatile("csrw mscratch, %0" : : "r" (x));
}

static inline void write_mtvec(uint32_t x) {
    asm volatile("csrw mtvec, %0" : : "r" (x));
}

static inline uint32_t get_mie(void) {
    uint32_t mie;
    asm volatile("csrr %0, scause" : : "=r" (mie));
    return mie;
}

static inline uint32_t get_sip(void) {
    uint32_t sip;
    asm volatile("csrr %0, sip" : : "=re" (sip));
    return sip;
}

static inline void write_sip(uint32_t x) {
    asm volatile("csrw sip, %0" : : "r" (x));
}

#endif
