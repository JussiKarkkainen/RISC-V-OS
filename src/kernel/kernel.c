#include "console.c"
#include "uart.h"
#include "../libc/include/stdio.h"
#include "../libc/stdio/kprintf.c"
#include "paging.h"

void enter(void) {
    kprintf("%s\n", "Booting OS");

    pmm_init();         // Initialize physical memory manager
    kpage_init();       // Initilaize kernel pagetable
    init_paging();      // Initialize paging
    
    console_init();     // Start console

}
