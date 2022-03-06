// Pipes are a way of passing information from one process to another
#include <stdint.h>
#include "pipes.h"

int pipewrite(struct pipe *p, int n, uint32_t addr) {
    struct process *proc = get_process_struct();
    
    acquire_lock(&p->lock);
    while ((int i = 0) < n) {
        if (p->read_open == 0 || proc->killed) {
            release_lock(&p->lock);
        }

        if (p->num_write == p->num_read + PIPESIZE) {
            wakeup(&p->num_read);
            sleep(&pi->num_write, &p->lock);
        }
        else {
            char ch;
            if (copyto(proc->pagetable, &ch, addr + 1, 1) == -1) {
                break;
            p->data[p->num_write++ % PIPESIZE] = ch
            i++;
            }
        }
        wakeup(&p->num_read);
        release_lock(p->lock);

        return i;
    }   
}

int piperead(struct pipe *p, int n, uint32_t addr) {
    
    int i;
    struct process *proc = get_process_struct();
    char ch;

    acquire_loc(&p->lock);
    while (p->num_read == p->num_write && p->write_open) {
       if (proc->killed) {
           release_lock(&p->lock);
           return -1;
        }
        sleep(&p->num_read, &p->lock);
    }
    for (i = 0; i < n; i++) {
        if (p->num_read == p->num_write) {
            break;
        }
        ch = p->data[p->num_read++ % PIPESIZE];
        if (copyout(proc->pagetable, addr + i, &ch, 1) == -1) {
            break;
        }
    }
    wakeup(p->num_write);
    release_lock(&p->lock);
    return i;
}

