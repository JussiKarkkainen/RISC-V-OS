#include <stdint.h>
#include "process.h"
#include "regs.h"
#include "locks.h"
#include "../libc/include/stdio.h"
#include "paging.h"
#include "../libc/include/string.h"
#include "filesys.h"
#include "file.h"

extern void transfer();
extern void forkret(void);

struct process *initproc;

struct process p[MAXPROC];

struct spinlock pid_lock;

struct spinlock wait_lock;

char uservec[];

int nextpid = 1;

unsigned char initcode[] = {
    0x17, 0x05, 0x00, 0x00, 0x13, 0x05, 0x45, 0x02,
    0x97, 0x05, 0x00, 0x00, 0x93, 0x85, 0x35, 0x02,
    0x93, 0x08, 0x70, 0x00, 0x73, 0x00, 0x00, 0x00,
    0x93, 0x08, 0x20, 0x00, 0x73, 0x00, 0x00, 0x00,
    0xef, 0xf0, 0x9f, 0xff, 0x2f, 0x69, 0x6e, 0x69,
    0x74, 0x00, 0x00, 0x24, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00
};

// First user process called from enter in kernel.c
void init_user(void) {
    
    struct process *proc;

    proc = alloc_process();
    initproc = proc;
    
    upaging_init(proc->pagetable, initcode, sizeof(initcode)); 
    proc->mem_size = PGESIZE;
    
    proc->trapframe->saved_pc = 0;
    proc->trapframe->kernel_sp = PGESIZE;

    strcpy(proc->name, "initcode", sizeof(proc->name));
    proc->cwd = name_inode("/");

    proc->state = RUNNABLE;

    release_lock(&proc->lock);
}

int alloc_pid(void) {

    int pid;
    acquire_lock(&pid_lock);
    pid = nextpid;
    nextpid = nextpid + 1;
    release_lock(&pid_lock);
    return pid;
}

uint32_t *proc_pagetable(struct process *proc) {
    
    uint32_t *pagetable;
    pagetable = upaging_create();

    if (kmap(pagetable, USERVEC, PGESIZE, (uint32_t)uservec, PTE_R | PTE_X) < 0) {
        uvmfree(pagetable, 0);
        return 0;
    }

    if (kmap(pagetable, TRAPFRAME, PGESIZE, (uint32_t)(proc->trapframe), PTE_R | PTE_W) < 0) {
        uvmunmap(pagetable, USERVEC, 1, 0);
        uvmfree(pagetable, 0);
        return 0;
    }
    return pagetable;
}

struct process *alloc_process(void) {
   
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

        if ((proc->trapframe = (struct trapframe *)zalloc(1)) == 0) {
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
        memset(&proc->context, 0, sizeof(proc->context));
        proc->context.ra = (uint32_t)forkret;
        proc->context.sp = (uint32_t)proc->kernel_stack + PGESIZE;

        return proc;
}   

int growproc(int n) {

    unsigned int sz;
    struct process *p = get_process_struct();

    sz = p->mem_size;
    if (n > 0) {
        if ((sz = uvmalloc(p->pagetable, sz, sz + n)) == 0) {
            return -1;
        }
    } 
    else if (n < 0) {
        sz = uvmdealloc(p->pagetable, sz, sz + n);
    }
    p->mem_size = sz;
    return 0;
}

int fork(void) {

    int i, pid;
    struct process *np;
    struct process *p = get_process_struct();

    // Allocate process.
    if ((np = alloc_process()) == 0) {
        return -1;
    }

    // Copy user memory from parent to child.
    if (uvmcopy(p->pagetable, np->pagetable, p->mem_size) < 0) {
        freeproc(np);
        release_lock(&np->lock);
        return -1;
    }
  
    np->mem_size = p->mem_size;

    // copy saved user registers.
    *(np->trapframe) = *(p->trapframe);

    // Cause fork to return 0 in the child.
    np->trapframe->a0 = 0;

    // increment reference counts on open file descriptors.
    for (i = 0; i < NUMFILE; i++) {
        if (p->openfile[i]) {
            np->openfile[i] = file_dup(p->openfile[i]);
        }
    }
    np->cwd = inode_dup(p->cwd);

    strcpy(np->name, p->name, sizeof(p->name));

    pid = np->process_id;

    release_lock(&np->lock);

    acquire_lock(&wait_lock);
    np->parent = p;
    release_lock(&wait_lock);

    acquire_lock(&np->lock);
    np->state = RUNNABLE;
    release_lock(&np->lock);

    return pid;
}

int wait(uint32_t addrs) {
    struct process *np;
    int havekids, pid;
    struct process *p = get_process_struct();

    acquire_lock(&wait_lock);

    while (1) {
        // Scan through table looking for exited children.
        havekids = 0;
        for (np = process; np < &process[MAXPROC]; np++) {
            if (np->parent == p) {
                // make sure the child isn't still in exit() or swtch().
                acquire_lock(&np->lock);
        

                havekids = 1;
                if (np->state == ZOMBIE) { 
                    // Found one.
                    pid = np->process_id;
                    if (addrs != 0 && copyout(p->pagetable, addrs, (char *)&np->exit_state, sizeof(np->exit_state)) < 0) {
                        release_lock(&np->lock);
                        release_lock(&wait_lock);
                        return -1;
                    }
                    freeproc(np);
                    release_lock(&np->lock);
                    release_lock(&wait_lock);
                    return pid;
                }
                release_lock(&np->lock);
            }
        }

        // No point waiting if we don't have any children.
        if (!havekids || p->killed) {
            release_lock(&wait_lock);
            return -1;
        }

        // Wait for a child to exit.
        sleep(p, &wait_lock);  //DOC: wait-sleep
    }
}

void forkret(void) {
    
    int first = 1;
    release_lock(&get_process_struct()->lock);

    if (first) {
        first = 0;
        filesys_init(ROOTDEV);
    }
    utrapret();
}

void proc_freepagetable(uint32_t *pagetable, uint32_t size) {

    uvmunmap(pagetable, USERVEC, 1, 0); 
    uvmunmap(pagetable, TRAPFRAME, 1, 0);
    uvmfree(pagetable, size);
}

void freeproc(struct process *proc) {
    if(proc->trapframe) {
        kfree((void*)proc->trapframe, 1);
    }
    proc->trapframe = 0;
    if(proc->pagetable) {
        proc_freepagetable(proc->pagetable, proc->mem_size);
    }
    proc->pagetable = 0;
    proc->mem_size = 0;
    proc->process_id = 0;
    proc->parent = 0;
    proc->name[0] = 0;
    proc->sleep_channel = 0;
    proc->killed = 0;
    proc->exit_state = 0;
    proc->state = UNUSED;
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

void reparent(struct process *proc) {
    struct process *p;
    for (p = proc; p < &process[MAXPROC]; p++) {
        if (p->parent == p) {
            p->parent = initproc;
            wakeup(initproc);
        }
    }
}

void exit(int status) {

    struct process *proc = get_process_struct();

    if (proc == initproc) {
        panic("init exiting");
    }

    // Close all open files.
    for (int fd = 0; fd < NUMFILE; fd++) {
        if (proc->openfile[fd]) {
            struct file *f = proc->openfile[fd];
            file_close(f);
            proc->openfile[fd] = 0;
        }
    }

    begin_op();
    inode_put(proc->cwd);
    end_op();
    proc->cwd = 0;

    acquire_lock(&wait_lock);

    // Give any children to init.
    reparent(p);

    // Parent might be sleeping in wait().
    wakeup(proc->parent);
  
    acquire_lock(&proc->lock);

    proc->exit_state = status;
    proc->state = ZOMBIE;

    release_lock(&wait_lock);

    // Jump into the scheduler, never to return.
    scheduler();
    panic("zombie exit"); 
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
