#include "queue_handler.h"
#include "../kernel/paging.c"

struct queue_entry push_to_queue(struct queue_head *q, void *data, int size) {

    struct queue_entry *qe = (struct queue_entry *)kalloc();
    if (!qe) {
        return 0;
    }
    qe->data = data;
    qe->size = size;
    qe->next = 0;
    
    if (q->tail) {
        q->tail->next = qe;
    }
    q->tail = qe;
    
    if (!q->next) {
        q->next = qe;
    }
    
    q->num++;
    return qe;
}



struct queue_entry get_from_queue(struct queue_head *q) {
}



