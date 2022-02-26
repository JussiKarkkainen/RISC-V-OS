#include <stdint.h>

static inline uint32_t read_tp(void) {
    uint32_t tp;
    asm volatile("mv %0, tp" : : "=r" (tp))
    return tp;
}

int which_cpu(void) {
    int cpu_id = read_tp()
    return cpu_id;
}
