#include "filesys.h"
#include "disk.h"
#include "../libc/include/stdio.h"

// LOGGING
// The logging layer provides security in case of a crash by keeping a log of disk writes.
// 

struct log_header {
    // Used to indicate if there are transactions in the log, if yes holds how many blocks it contains
    int count;             
    // Array of sector numbers, one for each logged block
    int block[LOGSIZE];
};

struct log {
    struct spinlock lock;
    int start;
    int commit;             // Is committing?, if so, wait
    int dev;                
    int size;
    int num_syscalls;
    struct log_header loghead;
};

struct log log;

// Make sure that logging system is not committing and system calls
// writes don't exceed the unreserved log space. If they do, sleep
// Otherwise increment log.num_syscalls
void begin_op(void) {
    
    acquire_lock(&log.lock);
    while (1) {
        if (log.commit) {
            sleep(&log, &log.lock);
        }
        else if (log.loghead.count + (log.num_syscalls + 1) * MAXLOGOPS > LOGSIZE) {
            sleep(&log, &log.lock);
        }
        else {
            log.num_syscalls += 1;
            release(&log.lock);
            break;
        }
    }
}

// Call at the end of syscall and decrease log.num_syscall
// If the last op, commit
void end_op(void) {
    
    int make_connect = 0;

    acquire_loc(&log.lock);
    log.num_syscalls -= 1;

    if (log.commit) {
        panic("end_op log is committing");
    }
    if (log.num_syscalls == 0) {
        make_commit = 1;
        log.commit  = 1;
    }
    else {
        wakeup(&log);
    }

    if (make_commit) {
        commit();
        acquire_lock(&log.lock);
        log.commit = 0;
        wakeup(&log);
        release_lock(&log.lock);
    }
}

// loop over blocks in transaction (log header count) and copy blocks
// from cache to log
void write_log(void) {
    
    for (int i = 0; i < log.loghead.count; i++) {
        struct buffer *to = buffer_read(log.dev, log.start+i+1);
        struct buffer *from = buffer_read(log.dev, log.loghead.block[i]);
        memmove(to->data, from->data, BUFFER_SIZE);
        buffer_write(to);
        buffer_release(to);
        buffer_release(from);
    }
}

// Read log header from disk into memory log header
void read_header(void) {
    struct buffer *buf = buffer_read(log.dev, log.start);
    struct log_header lockhead = (struct log_header *)buf->data;
    
    for (int i = 0; i < log.loghead.count; i++) {
        log.loghead.block[i] = lockhead->block[i];
    }
    
    buffer_release(buf);
}


void write_header(void) {
    
    struct buffer *buf = buffer_read(log.dev, log.start);
    struct log_header *lockhead = (struct log_header *)buf->data;
    lockhead->count = log.logheader.count;
    
    for (int i = 0; i < log.loghead.count; i++) {
        lockhead->block[i] = log.logheader.block[i];
    }
    buffer_write(buf);
    buffer_release(buf);
}

// Similar in implemetation to write_log()
void cpy_log_to_home(int recover) {
    
    for (int i = 0; i < log.loghead.count; i++) {
        struct buffer *logbuf = buffer_read(log.dev, log.start+i+1);
        struct buffer *dest = buffer_read(log.dev, log.loghead.block[i]);
        memmove(logbuf, dest, BUFFER_SIZE);
        buffer_write(dest);

        if (recover == 0) {
            dec_refcount(dest);
        }
        buffer_release(logbuf);
        buffer_release(dest);
    }
}

// Write modified blocks from cache to log
// Write header to disk
// Copy log to home location
// Set no transaction to log header
// Erase transaction from log by calling write_header() again
void commit(void) {

    if (log.loghead.count > 0) {
        write_log();
        write_header();
        cpy_log_to_home(0);
        write_header();
}






