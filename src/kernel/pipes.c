// Pipes are a way of passing information from one process to another
#include <stdint.h>
#include "pipes.h"
#include "process.h"
#include "file.h"
#include "paging.h"

int pipealloc(struct file **f0, struct file **f1) {
    struct pipe *pi;

    pi = 0;
    *f0 = *f1 = 0;
    if ((*f0 = file_alloc()) == 0 || (*f1 = file_alloc()) == 0) {
        goto bad;
    }
    if ((pi = (struct pipe*)zalloc()) == 0) {
        goto bad;
    }
    pi->read_open = 1;
    pi->write_open = 1;
    pi->num_write = 0;
    pi->num_read = 0;
    initlock(&pi->lock, "pipe");
    (*f0)->type = FD_PIPE;
    (*f0)->readable = 1;
    (*f0)->writable = 0;
    (*f0)->pipe = pi;
    (*f1)->type = FD_PIPE;
    (*f1)->readable = 0;
    (*f1)->writable = 1;
    (*f1)->pipe = pi;
    return 0;

    bad:
        if (pi) {
            kfree((uint32_t *)pi); 
        }
        if (*f0) {
            file_close(*f0);
        }
        if (*f1) {
            file_close(*f1);
        }
        return -1;
}

int pipewrite(struct pipe *p, int n, uint32_t addr) {
    struct process *proc = get_process_struct();
    
    int i = 0;

    acquire_lock(&p->lock);
    while (i < n) {
        if (p->read_open == 0 || proc->killed) {
            release_lock(&p->lock);
        }

        if (p->num_write == p->num_read + PIPESIZE) {
            wakeup(&p->num_read);
            sleep(&p->num_write, &p->lock);
        }
        else {
            char ch;
            if (copyto(proc->pagetable, &ch, addr + 1, 1) == -1) {
                break;
            p->data[p->num_write++ % PIPESIZE] = ch;
            i++;
            }
        }
        wakeup(&p->num_read);
        release_lock(&p->lock);

        return i;
    }   
}

void pipe_close(struct pipe *pi, int writable) {
  
    acquire_lock(&pi->lock);
    if (writable) {
        pi->write_open = 0;
        wakeup(&pi->num_read);
    } 
    else {
        pi->read_open = 0;
        wakeup(&pi->num_write);
    }
    if(pi->read_open == 0 && pi->write_open == 0) {
        release_lock(&pi->lock);
        kfree((uint32_t *)pi);
    } 
    else {
        release_lock(&pi->lock);
    }
}

int piperead(struct pipe *p, int n, uint32_t addr) {
    
    int i;
    struct process *proc = get_process_struct();
    char ch;

    acquire_lock(&p->lock);
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
    wakeup(&p->num_write);
    release_lock(&p->lock);
    return i;
}

