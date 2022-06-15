#ifndef QUEUE_HANDLER_H
#define QUEUE_HANDLER_H

struct queue_entry {
    void *data;
    int size;
    struct queue_head;
};

struct queue_head {
    struct queue_entry *next;
    struct queue_entry *tail;
    int num;
};


struct queue_entry push_to_queue(struct queue_head *q, void *data, int size);
struct queue_entry get_from_queue(struct queue_head *q);


#endif
