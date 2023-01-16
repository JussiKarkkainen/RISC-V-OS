#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>
#include "locks.h"
#include "filesys.h"

#define MAXCPUS 8
#define MAXPROC 64
#define NUMOFILES 16
#define ELF_MAGIC 0x464C457FU
#define ELF_PROG_LOAD 1

// Number of elements in array
#define NUM_ELEM(x) (sizeof(x) / sizeof((x)[0]))

// This is used in scheduling between processes
struct context {
    uint32_t ra;
    uint32_t sp;
    uint32_t s0;
    uint32_t s1;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
};

struct cpu {
    struct process *proc;
    struct context context;
    int depth_lock_intr_disable;
    int intr_prev_state;
};

enum proc_state {
    UNUSED,
    USED, 
    SLEEPING,
    RUNNABLE, 
    RUNNING,
    ZOMBIE
};

struct process {
    
    struct spinlock lock;
    
    enum proc_state state;
    void *sleep_channel;
    int killed;
    int exit_state;
    int process_id;

    struct process *parent;

    uint32_t kernel_stack;              // Adress of kernel stack
    uint32_t mem_size;
    
    uint32_t *pagetable;                // User page table
    struct trapframe *trapframe;
    struct context context;
    struct file *openfile[NUMOFILES];
    struct inode *cwd;                  // Current directory
    char name[16];    
};

extern struct cpu cpus[MAXCPUS];

struct trapframe {
    uint32_t kernel_satp;
    uint32_t kernel_sp;
    uint32_t kernel_trap;
    uint32_t saved_pc;
    uint32_t hartid;
    uint32_t ra;
    uint32_t sp;
    uint32_t gp;
    uint32_t tp;
    uint32_t t0;
    uint32_t t1;
    uint32_t t2;
    uint32_t s0;
    uint32_t s1;
    uint32_t a0;
    uint32_t a1;
    uint32_t a2;
    uint32_t a3;
    uint32_t a4;
    uint32_t a5;
    uint32_t a6;
    uint32_t a7;
    uint32_t s2;
    uint32_t s3;
    uint32_t s4;
    uint32_t s5;
    uint32_t s6;
    uint32_t s7;
    uint32_t s8;
    uint32_t s9;
    uint32_t s10;
    uint32_t s11;
    uint32_t t3;
    uint32_t t4;
    uint32_t t5;
    uint32_t t6;
};

struct elf_header {
    unsigned int magic;
    unsigned char elf[12];
    unsigned short type;
    unsigned short machine;
    uint32_t version;
    uint32_t entry;
    uint32_t phoff;
    uint32_t shoff;
    uint32_t flags;
    unsigned short ehsize;
    unsigned short phnum;
    unsigned short shentsize;
    unsigned short shnum;
    unsigned short shstrndx;
};

struct prog_header {
    uint32_t type;
    uint32_t off;
    uint32_t vaddr;
    uint32_t paddr;
    uint32_t filesz;
    uint32_t memsz;
    uint32_t flags;
    uint32_t align;
};

void process_init(void);
void map_kstack(uint32_t *pagetable);
struct cpu *get_cpu_struct(void);
struct process *get_process_struct(void);
int which_cpu(void); 
void sleep(void *sleep_channel, struct spinlock *lock);
void wakeup(void *sleep_channel);
void exit(int status);
void yield_process(void);
int either_copyin(void *dst, int user_src, uint32_t src, uint32_t len);
int either_copyout(int user_dst, uint32_t dst, void *src, uint32_t len);
void init_user(void);
struct process *alloc_process(void);
void forkret(void);
void freeproc(struct process *proc);
int wait(uint32_t addr);
int fork(void);
int growproc(int n);
int kill(int process_id);
void proc_freepagetable(uint32_t *pagetable, uint32_t size);
uint32_t *proc_pagetable(struct process *proc);
void cpu_scheduler(void);
void procdump(void);

#endif
