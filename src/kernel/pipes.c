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
    }
}



