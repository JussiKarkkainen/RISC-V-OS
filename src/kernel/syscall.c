#include <stdint.h>
#include "syscall.h"
#include "process.h"
#include "paging.h"
#include "../libc/include/stdio.h"
#include "../libc/include/stdio.h"

// Number of elements in array
#define NUM_ELEM(x) (sizeof(x) / sizeof((x)[0]))

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

int fetchaddr(uint32_t addr, uint32_t *ip) {

    struct process *proc;
    if (addr >= proc->size || addr+sizeof(uint32_t) > proc->state) {
        return -1;
    }
    if (copyto(proc->pagetable. (char *)ip, addr, sizeof(*ip) != 0)) {
        return -1;
    return 0;
}

int fetchstr(uint32_t addr, struct buffer *buf, int max) {
    
    struct process *proc;
    int err = copyinstr(p->pagetable, buf, addr, max);

    if (err < 0) {
        return err;
    }
    return strlen(buffer);
}

uint32_t argraw(int n) {
    struct process *proc;
    switch (n) {
        case 0:
            return proc->trapframe->a0;
        case 1:
            return proc->trapframe->a1;
        case 2: 
            return proc->trapframe->a2;
        case 3:
            return proc->trapframe->a3;
        case 4:
            return proc->trapframe->a4;
        case 5: 
            return proc->trapframe->a5;
        
        default:
            panic("argraw");
    }
    return -1;
}

int argint(int n, int *ip) {
    *ip = argraw(n);
    return 0;
}

int argaddr(int n, uint32_t *ip) {
    *ip = argraw(n);
    return 0;
}

int argstr(int n, char *buf, int max) {
    uint32_t addr;
    if (argaddr(, &addr) < 0) {
        return -1;
    }
    return fetchstr(addr, buf, max);
}

// Handle syscalls, is called from utrap() in trap.c
void handle_syscall(void) {
    
    int syscall_num;
    struct process *proc = get_process_struct();
    
    // Syscall num is stored in the a7 register
    syscall_num = proc->trapframe->a7;
    
    // Syscalls is an array of function pointers
    // Check if given num exists and execute syscall
    if (syscall_num > 0 && syscall_num < (int)NUM_ELEM(syscall) && syscall[syscall_num]) {
        proc->trapframe->a0 = syscall[syscall_num]();
    }
    // Syscall not recognized, print for debug, return -1 for failure
    else {
        kprintf("unknown syscall %d, %s, %d", proc->process_id, proc->name, syscall_num);
        proc->trapframe->a0 = -1;
    }
}

