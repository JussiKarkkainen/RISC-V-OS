#include "uart.h"
#include "../libc/include/stdio.h"
#include "paging.h"

// This should be executed in supervisor mode, boot.S should first call 
// start() for setup before tranfering control to enter().

void enter(void) {
    kprintf("%s\n", "Booting OS");

    pmm_init();         // Initialize physical memory manager
    kpage_init();       // Initilaize kernel pagetable
    init_paging();      // Initialize paging
//    init_ktrapvec();    // Write ktrapvec addr into stvec to init trap handling
    plic_init();        // Setup interrupt controller
    plic_init_hart();   // request device interrupts
    console_init();     // Start console
}
