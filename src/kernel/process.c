#include <stdint.h>
#include "process.h"
#include "regs.h"
#include "locks.h"
#include "../libc/include/stdio.h"

extern void transfer();

struct process *p[MAXPROC];

struct spinlock pid_lock;

// First user process called from enter in kernel.c
void init_user(void) {
    
    struct process *proc;

    proc = alloc_process();

struct process *alloc_process() {
   
    struct process *proc;

    for (proc = p; proc < &p[MAXPROC]; proc++) {
        acquire_lock(&proc->lock);
        if (proc->state == UNUSED) {
            goto found;
        }
        else {
            release_lock(&proc->lock);
        }
    }
    return 0;
    
    found:
        proc->process_id = alloc_pid();
        proc->state = USED;

        if ((proc->trapframe = (struct trapframe *)zalloc()) == 0) {
            freeproc(proc);
            release_lock(&proc->lock);
            return 0;
        }

        proc->pagetable = proc_pagetable(proc);
        if (proc->pagetable == 0) {
            freeproc(proc);
            release_lock(&proc->lock);
            return 0;
        }
        memset(&proc->context, 0, sizeof(proc->contexti));
        proc->context.ra = (uint32_t)forkret;
        proc->contect.sp = (uint32_t)proc->kstack + PGESIZE;

        return proc;
}   

int alloc_pid(void) {

    int pid;
    acquire_lock(&pid_lock);
    pid = nextpid;
    nextpid = nextpid + 1;
    release_lock(&pid_lock);
    return pid;
}

// Wake up processes sleeping on sleep channel
void wakeup(void *sleep_channel) {
    struct process *proc;

    for (proc = process; proc < &process[MAXPROC]; proc++) {
        if (proc != get_process_struct()) {
            acquire_lock(&proc->lock);
            if (proc->state == SLEEPING && proc->sleep_channel == sleep_channel) {
                proc->state = RUNNABLE;
            }
            release_lock(&proc->lock);
        }
    }
}

// This is the scheduler for CPUS. Its called from enter()
// and runs in an infinite loop. 
// 1. Choose process to run
// 2. call transfer() to start running that process
// 3. Transfer eventually returns control
void cpu_scheduler(void) {

    struct process *proc;
    struct cpu *cpu = get_cpu_struct();
    cpu->proc = 0;
    
    while (1) {
        enable_intr();

        for (proc = process; proc < &process[MAXPROC]; proc++) {
            acquire_lock(&proc->lock);
            if (proc->state == RUNNABLE) {
                
                proc->state = RUNNING;
                transfer(&cpu->context, &proc->context);

                cpu->proc = 0;
            }
            release_lock(&proc->lock);
        }
    }
}


void scheduler(void) {
    int intr_prev_state;
    struct process *proc = get_process_struct();

    if (!is_holding(&proc->lock)) {
        panic("scheduler, is_holding");
    }
    if (get_cpu_struct()->depth_lock_intr_disable != 1) {
        panic("scheduler, depth_lock_intr_disable");
    }
    if (proc->state == RUNNING) {
        panic("scheduler, running process");
    }
    if (get_intr()) {
        panic("scheduler, interrupts");
    }

    intr_prev_state = get_cpu_struct()->intr_prev_state;
    transfer(&proc->context, &get_cpu_struct()->context);
    get_cpu_struct()->depth_lock_intr_disable = intr_prev_state;
}

void yield_process(void) {
    struct process *proc = get_process_struct();
    acquire_lock(&proc->lock);
    proc->state = RUNNABLE;
    scheduler();
    release_lock(&proc->lock);
}

void sleep(void *sleep_channel, struct spinlock *lock) {
    struct process *proc = get_process_struct();
    
    acquire_lock(&proc->lock);
    release_lock(lock);
    
    // Put process to sleep
    proc->sleep_channel = sleep_channel;
    proc->state = SLEEPING;

    scheduler();

    proc->sleep_channel = 0;
    release_lock(&proc->lock);
    acquire_lock(lock);
}

int kill(int process_id) {
    
    struct process *proc;
    for (proc = process; proc < &process[MAXPROC]; proc++) {
        acquire_lock(&proc->lock);
        
        if (proc->process_id == process_id) {
            proc->killed = 1;
            if (proc->state == SLEEPING) {
                proc->state = RUNNABLE;
            }
            
            release_lock(&proc->lock);
            return 0;
        }
        release_lock(&proc->lock);
    }
    return -1;
}

void exit(int status) {
}

int which_cpu(void) {
    int cpu_id = get_tp();
    return cpu_id;
}

int either_copyin(void *dst, int user_src, uint32_t src, uint32_t len) {
    
    struct process *proc = get_process_struct();
    if(user_src) {
        return copyto(proc->pagetable, dst, src, len);
    } 
    else {
        memmove(dst, (char*)src, len);
        return 0;
    }
}

int either_copyout(int user_dst, uint32_t dst, void *src, uint32_t len) {
    
    struct process *proc = get_process_struct();
    if (user_dst) {
        return copyout(proc->pagetable, dst, src, len);
    }
    else {
        memmove((char *)dst, src, len);
        return 0;
    }
}

// Fetch current cpu struct
struct cpu *get_cpu_struct(void) {
    int hart_id = which_cpu();
    struct cpu *c = &cpus[hart_id];
    return c;
}

// Fetch current process struct
struct process *get_process_struct(void) {
    lock_intr_disable();
    struct cpu *c = get_cpu_struct();
    struct process *p = c->proc;
    return p;
}
