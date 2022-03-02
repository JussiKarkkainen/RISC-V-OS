#include "trap.h"
#include "../libc/include/stdio.h"
#include "process.h"
#include "regs.h"

int handle_device_intr() {
    // Check if external/device interrupt
    uint32_t scause = get_scause();
    if (((scause & INTERRUPT_BIT) == 1) && ((scause & EXT_INTERRUPT) == 9)) {  
        // Interrupt given by PLIC
        int intr_id = plic_read();

        // Check if interrupt is from uart, disk or neither
        if (intr_id == UART_INTR) {
            uart_intr();
        }
        else if (intr_id == VIRTIO_DISK) {
            virtio_disk_intr();
        }
        else if (intr_id) {
            kprintf("Device interrupt not recognizedc: %d\n", intr_id);
        }

        // Tell PLIC its allowed to send interrutps again
        if (intr_id) {
            plic_finished(intr_id);
        }

        return 0;
    }
  
    // Check if software/timer interrupt
    else if (scause == SOFTWARE_INTR) {
        // Device timer interrupts get turned into software interrupts

        if (which_cpu() == 0) {
            timer_interrupt();
        }

        // Clear the pending software interrupt        
        write_sip(get_sip() & CLEAR_SIP_SSIP);
        return 1;
    }
    else {
        return 2;
    }

}

int timer_interrupt(void) {
    
    acquire_lock(&ticklock);
    ticks++;
    wakeup(&ticks);
    release_lock(&ticklock);
}

void yield_process(void) {
}

void ktrapvec();

void utrap(void) {
    uint32_t sstatus = get_sstatus(); 
    uint32_t scause = get_scause();
    int intr_result;

    // Check if trap comes from user mode
    if ((sstatus & SSTATUS_SPP) == 0) {
        panic("trap not from user mdoe");
    }

    // send interrupts and exceptions to ktrap
    write_stvec((uint32_t)ktrapvec);

    // get process struct
    struct process *proc = get_process_struct():

    // save user pc
    proc->trapframe->saved_pc = read_sepc(); 
    
    // check if syscall
    if (scause == 8) {
        // Return to next instruction 
        proc->trapframe->saved_pc += 4;
        
        enable_intr();

        handle_syscall();
    }
    // check if device interrupt and handle with handle_device_intr()
    if ((intr_result = handle_device_intr()) == 2) {
        kprintf("Unexpexted scause in utrap(), scause: %x\n, sepc: %x\n, stval: %x\n", 
                get_scause(), get_sepc(), get_stval());
    } 
    // Otherwise kill process
    proc->trapframe->killed = 1;
    // Check if timer interrupt 
    if (intr_result = 1) {
        yield_process(); 
    
    // Call utrapret
    utrapret();
}

void utrapret(void) {

    struct process *proc = get_process_struct();
    disable_intr();
    
    // Send traps to utrapvec
    write_stvec(UTRAPVEC);
    
    // Utrapvec will need these register values
    proc->trapframe->kernel_satp = get_satp();         
    proc->trapframe->kernel_sp = proc->kernel_stack + 4096;
    proc->trapframe->kernel_trap = (uint32_t)usertrap;
    proc->trapframe->hartid = get_tp();
    
    // Set previous privilige mode to user
    uint32_t sstatus = get_sstatus();
    // Clear SIE and set SPIE
    write_sstatus(((sstatus & SSTATUS_SIE_CLEAR) | SSTATUS_SPIE));

    // Set exception program counter to saved user pc
    write_sepc(proc->trapframe->saved_pc);
 
}

void ktrap(void) {

    uint32_t sepc = get_sepc();
    uint32_t sstatus = get_sstatus();
    uint32_t scause = get_scause();
    uint32_t stval = get_stval();
    int intr_result;

    // Make sure interrupt comes from supervisor mode
    if ((sstatus & SSTATUS_SPP) == 0) {
        panic("trap not in supervisor mode");
    }
    // Make sure interrupts are not enabled
    if ((sstatus & SSTATUS_SIE) == 1) {
        panic("interrupts are enabled");
    }

    // trap can be either device interrupt or exceptions.
    // handle_interrupt deals with device interrupt. If trap is
    // an external interrupt, we call panic() and stop executing
    if ((intr_result = handle_device_intr()) == 2) {
        // Print out register info and panic
        kprintf("scause: %x\n, sstatus: %x\n, stval: %x\n", scause, sstatus, stval);
        panic("kernel interrupt");
    }
        
    if (intr_result == 1) {
        yield_process();

    // Restore trap registers if changed by yield_process()
    write_sepc(sepc);
    write_sstatus(sstatus);
    }
}

void ktrapvec();


void init_ktrapvec(void) {
    write_stvec((uint32_t)ktrapvec);
}

