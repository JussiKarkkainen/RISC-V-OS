#include <stdint.h>
#include "syscall.h"
#include "process.h"

// Handle syscalls, is called from utrap() in trap.c
void syscall(void) {
    
    int syscall_num;
    struct process *proc = get_process_struct();
    
    // Syscall num is stored in the a7 register
    num = proc->trapframe->a7;
    
    // Syscalls is an array of function pointers
    // Check if given num exists and execute syscall
    if (num > 0 && num < NUM_ELEM(syscalls) && syscalls[num]) {
        p->trapframe->a0 = syscalls[num]();
    }
    // Syscall not recognized, print for debug, return -1 for failure
    else {
        kprintf("unknown syscall %d, %s, %d", proc->process_id, proc->name, num);
        proc->trapframe->a0 = -1;
}

