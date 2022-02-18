#include "arch/riscv/tty.c"
#include "arch/riscv/uart.h"
#include "../libc/include/stdio.h"
#include "../libc/stdio/kprintf.c"
#include "arch/riscv/mem.S"
//#include "pmm.c"
//#include "paging.h"
//#include "paging.c"

void enter(void) {
    kprintf("%s\n", "Booting OS");

    //pmm_init();         // Initialize physical memory manager
    kpage_init();       // Initilaize kernel pagetable
    init_paging();      // Initialize paging
    
    console_init();     // Start console

}
