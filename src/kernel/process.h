#ifndef PROCESS_H
#define PROCESS_H

#include <stdint.h>

#define MAXCPUS 8

struct cpu {
    struct process *proc;
    struct context context;
    int depth_lock_intr_disable;
    int intr_prev_state;
};

// This os supports maximum 8 cpu cores, make cpu struct for all
struct cpu cpus[MAXCPUS];

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
    int killed;
    int exit_state;
    int process_id;

    struct proc *parent;
    
    uint32_t kernel_stack;          // Adress of kernel stack
    uint32_t mem_size;
    uint32_t *pagetable;            // User page table
    struct trapframe *trapframe;
    struct context context;
    char name[16];    
};


struct trapframe {
    uint32_t kernel_pagetable;
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

struct context {
};


static inline uint32_t read_tp(void); 
int which_cpu(void); 

#endif
