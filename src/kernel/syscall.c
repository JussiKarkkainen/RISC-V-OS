#include <stdint.h>
#include "syscall.h"
#include "process.h"
#include "../libc/include/stdio.h"

uint32_t (*syscall[])(void) = {
    [SYS_FORK] sys_fork,
    [SYS_EXIT] sys_exit,
    [SYS_WAIT] sys_wait,
    [SYS_PIPE] sys_pipe,
    [SYS_READ] sys_read,
    [SYS_KILL] sys_kill,
    [SYS_EXEC] sys_exec,
    [SYS_FSTAT] sys_fstat,
    [SYS_CHDIR] sys_chdir,
    [SYS_DUP] sys_dup,
    [SYS_GETPID] sys_getpid,
    [SYS_SBRK] sys_sbrk,
    [SYS_SLEEP] sys_sleep,
    [SYS_UPTIME] sys_uptime,
    [SYS_OPEN] sys_open,
    [SYS_WRITE] sys_write,
    [SYS_MKNOD] sys_mknod,
    [SYS_UNLINK] sys_unlink,
    [SYS_LINK] sys_link,
    [SYS_MKDIR] sys_mkdir,
    [SYS_CLOSE] sys_close
};


// Handle syscalls, is called from utrap() in trap.c
void handle_syscall(void) {
    
    int syscall_num;
    struct process *proc = get_process_struct();
    
    // Syscall num is stored in the a7 register
    sycall_num = proc->trapframe->a7;
    
    // Syscalls is an array of function pointers
    // Check if given num exists and execute syscall
    if (syscall_num > 0 && syscall_num < NUM_ELEM(syscall) && syscall[syscall_num]) {
        proc->trapframe->a0 = syscall[syscall_num]();
    }
    // Syscall not recognized, print for debug, return -1 for failure
    else {
        kprintf("unknown syscall %d, %s, %d", proc->process_id, proc->name, syscall_num);
        proc->trapframe->a0 = -1;
    }
}

