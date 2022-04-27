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
        
        console_init();     // Start console
        kprintf_init();
        kprintf("%s\n", "Booting OS");

        pmm_init();         // Initialize physical memory manager
        kpage_init();       // Initilaize kernel pagetable
        init_paging();      // Initialize paging
        process_init();     // init process table
        init_trapvec();     // Initialize timer trapvec
        init_ktrapvec();    // Write ktrapvec addr into stvec to init trap handling
        plic_init();        // Setup interrupt controller  
        plic_init_hart();   // request device interrupts
        buffer_init();      // Initialize the buffer cache for filesystem
        inode_init();       // Initialize the inode table
        file_init();        // Initialize file table
        disk_init();        // Initialize virtio disk
        init_user();
        __sync_synchronize();
//        started = 1;
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
    cpu_scheduler();
}
