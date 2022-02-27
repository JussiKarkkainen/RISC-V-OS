#define PLIC 0xc000000
#define PLIC_CLAIM_OFFSET 0x201004

void plic_init(void) {
    
}

void plic_init_hart(void) {
    
}

int plic_read(void) {
    int hart_id = which_cpu();
    // get interrupt ids from plic
    intr_id = (PLIC + PLIC_CLAIM_OFFSET + hart_id * 0x2000);
    return intr_id;
}

void plic_finished(int intr_id) {
    int hart_id = which_cpu();
    (PLIC + PLIC_CLAIM_OFFSET + hart_id * 0x2000) = intr_id;
}
