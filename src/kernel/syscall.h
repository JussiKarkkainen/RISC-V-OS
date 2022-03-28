#ifndef SYSCALL_H
#define SYSCALL_H

#include <stdint.h>

#define SYS_FORK 1
#define SYS_EXIT 2
#define SYS_WAIT 3
#define SYS_PIPE 4
#define SYS_READ 5
#define SYS_KILL 6
#define SYS_EXEC 7
#define SYS_FSTAT 8
#define SYS_CHDIR 9
#define SYS_DUP 10
#define SYS_GETPID 11
#define SYS_SBRK 12
#define SYS_SLEEP 13
#define SYS_UPTIME 14
#define SYS_OPEN 15
#define SYS_WRITE 16
#define SYS_MKNOD 17
#define SYS_UNLINK 18
#define SYS_LINK 19
#define SYS_MKDIR 20
#define SYS_CLOSE 21

#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400

void handle_syscall(void);
int argint(int n, int *ip);
int fetchaddr(uint32_t addr, uint32_t *ip);
int fetchstr(uint32_t addr, struct buffer *buf, int max);
int argaddr(int n, uint32_t *ip);
int argstr(int n, char *buf, int max);
uint32_t argraw(int n);


// file syscalls
int argfd(int n, int *pfd, struct file **pf);
uint32_t sys_exec(void);
uint32_t sys_write(void);
uint32_t sys_read(void);
uint32_t sys_close(void);
uint32_t sys_chdir(void);
uint32_t sys_mknod(void);
uint32_t sys_mkdir(void);
uint32_t sys_open(void);
uint32_t sys_unlink(void);
uint32_t sys_link(void);
uint32_t sys_fstat(void);
uint32_t sys_open(void);
uint32_t sys_dup(void);
uint32_t sys_pipe(void);

// process syscalls
uint32_t sys_exit(void);
uint32_t sys_getpid(void);
uint32_t sys_fork(void);
uint32_t sys_wait(void);
uint32_t sys_sbrk(void);
uint32_t sys_sleep(void);
uint32_t sys_kill(void);
uint32_t sys_uptime(void);

#endif
