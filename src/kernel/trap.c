#include "trap.h"
#include "../libc/stdio/panic.c"


int handle_device_intr() {
    // Check if external/device interrupt
    uint32_t scause = get_scause();
    if ((scause & INTERRUPT_BIT) == 1 && (scause & EXT_INTERRUPT == 9)) {  
        // Interrupt given by PLIC
        intr_id = plic_read();

        // Check if interrupt is from uart, disk or neither
        if (intr_id == UART_INTR) {
            uart_intr();
        }
        else if (intr_id == VIRTIO_DISK) {
            virtio_disk_intr();
        }
        else {
            kprintf("Device interrupt not recongnized");
        }

        // Tell PLIC its allowed to send interrutps again
        if (intr_id) {
            plic_finished(intr_id);
        }

        return 0;
    }
  
    // Check if software/timer interrutp
    else if (scause == SOFTWARE_INTR) {
        

    }
}

int timer_interrupt() {
}

void yield_process(void) {
}

void utrap(void) {
    uint32_t sstatus = get_sstatus(); 
    uint32_t scause = get_scause();

    // Check if trap comes from user mode
    if ((sstatus & SSTATUS_SPP) == 0) {
        panic("trap not from user mdoe");
    }

    // send interrupts and exceptions to ktrap
    write_stvec((uint32_t)ktrapvec);
    // save user pc
    
    // check if syscall and handle with funct

    // check if device interrupt and handle with handle_device_intr()

    // Otherwise kill process

    // Check if timer interrutp 
   
    // Call utrapret 
}



void ktrap(void) {

    uint32_t sepc = get_sepc();
    uint32_t sstatus = get_sstatus();
    uint32_t scause = get_scause();
    uint32_t stval = get_stval():

    // Make sure interrupt comes from supervisor mode
    if ((sstatus & SSTATUS_SPP) == 0) {
        panic("trap not in supervisor mode");
    }
    // Make sure interrupts are not enabled
    if ((sstatus & SSTATUS_SIE) == 1) {
        panic("interrupts are enabled");
    }

    // trap can be either device interrupt or exceptions
    // handle_interrupt deals with device interrupt. If trap is
    // an external interrupt, we call panic() and stop executing
    if (handle_device_intr() == 0) {
        // Print out register info and panic
        kprintf("scause: %x\n, sstatus: %x\n, stval: %x\n", scause, sstatus, stval);
        panic("kernel interrupt");
    }
        

    if (timer_interrupt() == 1) {
        yield_process();
    }
}

void ktrapvec();


void init_ktrapvec(void) {
    write_stvec((uint32_t)ktrapvec);
}

