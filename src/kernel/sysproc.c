#include "syscall.h"
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
}


uint32_t sys_fork(void) {
}


uint32_t sys_wait(void) {
}


uint32_t sys_sbrk(void) {
}


uint32_t sys_sleep(void) {
}


uint32_t sys_kill(void) {
}


uint32_t sys_uptime(void) {
}






