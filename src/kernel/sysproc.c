#include "syscall.h"
#include "process.h"
#include "locks.h"
#include "trap.h"
#include <stdint.h>

uint32_t sys_exit(void) {

    int n;
    if (argint(0, &n) < 0) {
        return -1;
    }
    exit(n);
    return 0;
}

uint32_t sys_getpid(void) {

    return get_process_struct()->process_id;
}

uint32_t sys_fork(void) {

    return fork();
}

uint32_t sys_wait(void) {

    uint32_t i;
    if (argaddr(0, &i) < 0) {
        return -1;
    }
    return wait(i);
}

uint32_t sys_sbrk(void) {

    int addr, n;
    if (argint(0, &n) < 0) {
        return -1;
    }
    addr = get_process_struct()->mem_size;
    if (growproc(n) < 0) {
        return -1;
    }
    return addr;
}

uint32_t sys_sleep(void) {
   
   int n;
   unsigned int ticks0;

   if (argint(0, &n) < 0) {
       return -1;
    }
    acquire_lock(&tickslock);
    ticks0 = ticks;
    while(ticks - ticks0 < n) {
        if (get_process_struct()->killed) {
            release_lock(&tickslock);
            return -1;
        }
        sleep(&ticks, &tickslock);
    }
    release_lock(&tickslock);
    return 0;
}

uint32_t sys_kill(void) {
    int pid;
    
    if (argint(0, &pid) < 0) {
        return -1;
    }
    return kill(pid);
}

uint32_t sys_uptime(void) {

    unsigned int xticks;

    acquire_lock(&tickslock);
    xticks = ticks;
    release_lock(&tickslock);
    return xticks;
}
