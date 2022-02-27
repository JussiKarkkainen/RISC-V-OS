#define PLIC 0xc000000
#define PLIC_CLAIM_OFFSET 0x201004

void plic_init(void) {
}

int plic_read(void) {
    hart_id = which_cpu();
    intr_id = (PLIC + PLIC_CLAIM_OFFSET + hart_id * 0x2000);
    return intr_id;
}

int plic_finished(int intr_id) {
}
