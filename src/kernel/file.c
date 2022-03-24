#include "file.h"
#include <stdint.h>

// File descriptor layer
// These functions are used in file system system calls

struct {
    struct spinlock lock;
    struct file file[NUMFILE];
}file_table;

void file_init(void) {
    initlock(&file_table.lock, "File table");
}

struct file *file_alloc(void) {
    
    struct file *file;

    acquire_lock(&file_table.lock);
    for (file = file_table.file; file < file_table.file + NUMFILE; file++) {
        if (file->ref == 0) {
            file->ref = 1;
            release_lock(&file_table.lock);
            return file;
        }
    }
    release_lock(&file_table.lock);
    return 0;
}

struct file *file_inc(struct file *file) {
    
    acquire_lock(&file_table.lock);
    if (file->ref < 1) {
        panic("file_inc, file->ref < 1");
    }
    file->ref++;
    release_lock(&file_table.lock);
    return file;
}

void file_clode(struct file *file) {

    struct file *f;

    acquire_lock(&file_table.lock);
    if (file->ref < 1) {
        panic("file_close, file->ref < 1");
    }
    if (--file->ref > 0) {
        release_lock(&file_table.lock);
        return;
    }

    f = *file;
    file->ref = 0;
    file->type = FD_NONE;
    release_lock(&file_table.lock);

    if (f.type == FD_PIPE) {
        pipe_close(f.pipe, f.writable);
    }
    else if {
        begin_op();
        inode_put(f.inode);
        end_op();
    }
}






















