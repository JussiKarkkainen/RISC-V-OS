// Setup cpu for supervisor mode
// Include enter

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

void mstart(void) {
    // Clear the mstatus MPP bits and set them to supervisor mode
    uint32_t mstatus = get_mstatus();
    mstatus &= ~(3 << 11);
    msstatus |= (1 << 11);

    // Set mepc to point to enter(), so when we call mret, execution jumps there
    write_mepc((uint32_t)enter);

    // Disable paging
    write_satp(0);
