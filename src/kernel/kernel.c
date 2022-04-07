#include "uart.h"
#include "../libc/include/stdio.h"
#include "paging.h"
#include "process.h"
#include "trap.h"
#include "plic.h"
#include "filesys.h"
#include "file.h"
#include "syscall.h"
#include "disk.h"

// This should be executed in supervisor mode, boot.S should first call 
// start() for setup before tranfering control to enter().
static volatile int started = 0;

void enter(void) {  
    
    if (which_cpu() == 0) {
        
        kprintf_init();
        kprintf("%s\n", "Booting OS");

        pmm_init();         // Initialize physical memory manager
//        test_alloc();
        kpage_init();       // Initilaize kernel pagetable
        init_paging();      // Initialize paging
        init_ktrapvec();    // Write ktrapvec addr into stvec to init trap handling
        init_trapvec();     // Initialize timer trapvec
        plic_init();        // Setup interrupt controller  
        plic_init_hart();   // request device interrupts

        buffer_init();      // Initialize the buffer cache for filesystem
        inode_init();       // Initialize the inode table
        file_init();        // Initialize file table
        init_user();
        console_init();     // Start console
        __sync_synchronize();
        started = 1;    
    }
    else {
        while (started == 0) {
            ;
        }
        __sync_synchronize();
        kprintf("hart %d booting\n", which_cpu());
        init_paging();
        init_ktrapvec();    
        plic_init_hart();
    }
}
