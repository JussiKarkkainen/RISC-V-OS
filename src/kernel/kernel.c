#include "arch/riscv/tty.c"
#include "arch/riscv/uart.h"
#include "../libc/include/stdio.h"
#include "../libc/stdio/kprintf.c"
#include "pmm.c"
//#include "paging.h"
//#include "paging.c"

void enter(void) {
    kprintf("%s\n", "Booting OS");
    
    pmm_init();

    //kpage_init();
    //init_paging();
    console_init();

}
