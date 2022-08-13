#include "trap.h"
#include "../libc/include/stdio.h"
#include "process.h"
#include "regs.h"
#include "uart.h"
#include "disk.h"
#include "syscall.h"
#include "plic.h"
#include "paging.h"

struct spinlock tickslock;
unsigned int ticks;

extern char uvec[], utrapvec[], utrapreturn[];

void ktrapvec();

extern int handle_device_intr();

int handle_device_intr(void) {
    // Check if external/device interrupt
    uint32_t scause = get_scause();
    if (((scause & 0x80000000L)) && ((scause & EXT_INTERRUPT) == 9)) {  
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

        plic_finished(intr_id);
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

void timer_interrupt(void) {
    
    acquire_lock(&tickslock);
    ticks++;
    wakeup(&ticks);
    release_lock(&tickslock);
}

void utrap(void) {
    uint32_t sstatus = get_sstatus(); 
    int intr_result = 2;
    
    // Check if trap comes from user mode
    if ((sstatus & SSTATUS_SPP) != 0) {
        panic("trap not from user mdoe");
    }

    // send interrupts and exceptions to ktrap
    write_stvec((uint32_t)ktrapvec);

    // get process struct
    struct process *proc = get_process_struct();

    // save user pc
    proc->trapframe->saved_pc = get_sepc(); 
    
    // check if syscall
    if (get_scause() == 8) {
        // Return to next instruction 
         
        if (proc->killed) {
            exit(-1);
        }
    
        proc->trapframe->saved_pc += 4;
        
        enable_intr();

        handle_syscall();
    
    } else if ((intr_result = handle_device_intr()) != 2) {

    } else {
        kprintf("Unexpexted scause in utrap(), scause: %p\n, sepc: %p\n, stval: %p\nname %s\n", 
                 get_scause(), get_sepc(), get_stval(), proc->name);
        proc->killed = 1;
    }
    // Otherwise kill process
    if (proc->killed) {
        exit(-1);
    }
    // Check if timer interrupt 
    if (intr_result == 1) {
        yield_process(); 
    } 
    // Call utrapret
    utrapret();
}

void utrapret(void) {

    struct process *proc = get_process_struct();
    disable_intr();
    
    // Send traps to utrapvec
    write_stvec(USERVEC + (utrapvec - uvec));
    
    // Utrapvec will need these register values
    proc->trapframe->kernel_satp = get_satp();         
    proc->trapframe->kernel_sp = proc->kernel_stack + PGESIZE;
    proc->trapframe->kernel_trap = (uint32_t)utrap;
    proc->trapframe->hartid = get_tp();
    // Set previous privilige mode to user
    uint32_t sstatus = get_sstatus();
    sstatus &= ~SSTATUS_SPP;
    sstatus |= SSTATUS_SPIE;
    write_sstatus(sstatus);

    // Set exception program counter to saved user pc
    write_sepc(proc->trapframe->saved_pc);

    uint32_t satp = MAKE_SATP(proc->pagetable);

    uint32_t fn = USERVEC + (utrapreturn - uvec);
    ((void (*)(uint32_t,uint32_t))fn)(TRAPFRAME, satp);
}

void ktrap(void) {
    uint32_t sepc = get_sepc();
    uint32_t sstatus = get_sstatus();
    uint32_t scause = get_scause();
    uint32_t stval = get_stval();
    int intr_result = 2;
    
    // Make sure interrupt comes from supervisor mode
    if ((sstatus & SSTATUS_SPP) == 0) {
        panic("trap not in supervisor mode");
    }
    /*
    // Make sure interrupts are not enabled
    if (sstatus & SSTATUS_SIE) {
        panic("interrupts are enabled");
    }
    */
    if (get_intr() != 0) {
        panic("trap not in supervisor mode");
    }

    if ((intr_result = handle_device_intr()) == 2) {
        // Print out register info and panic
        kprintf("scause: %p\nsstatus: %p\nstval: %p\nsepc %p\n", scause, sstatus, stval, sepc);
        panic("kernel interrupt, ktrap");
    }
     
    if (intr_result == 1 && get_process_struct() != 0 && get_process_struct()->state == RUNNING) {
        yield_process();
    }

    // Restore trap registers if changed by yield_process()
    write_sepc(sepc);
    write_sstatus(sstatus);
    
}

void init_trapvec(void) {
    initlock(&tickslock, "timer lock");
}

void init_ktrapvec(void) {
    write_stvec((uint32_t)ktrapvec);
}

