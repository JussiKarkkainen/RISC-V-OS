#include "file.h"
#include <stdint.h>
#include "locks.h"
#include "process.h"
#include "filesys.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include "pipes.h"
#include "paging.h"

// File descriptor layer
// These functions are used in file system system calls

struct devsw devsw[NUMDEV];

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

void file_close(struct file *file) {

    struct file f;

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
    else if (f.type == FD_INODE || f.type == FD_DEVICE) {
        begin_op();
        inode_put(f.inode);
        end_op();
    }
}

int file_stat(struct file *file, uint32_t address) {

    struct process *proc = get_process_struct();
    struct stat stat;

    if (file->type == FD_INODE || file->type == FD_DEVICE) {
        inode_lock(file->inode);
        copy_stat_inode(file->inode, &stat);
        inode_unlock(file->inode);
        if (copyout(proc->pagetable, address, (char *)&stat, sizeof(stat)) < 0) {
            return -1;
        }
        return 0;
    }
    return -1;
}

int read_file(struct file *file, uint32_t address, int n) {

    int a = 0;

    if (file->readable == 0) {
        return -1;
    }

    if (file->type == FD_PIPE) {
        a = piperead(file->pipe, address, n);
    }
    else if (file->type == FD_DEVICE) {
        if(file->major_dev_num < 0 || file->major_dev_num >= NUMDEV || !devsw[file->major_dev_num].read) {
            return -1;
        }
    
    a = devsw[file->major_dev_num].read(1, address, n);
    
    }
    else if (file->type == FD_INODE) {
        inode_lock(file->inode);
        if ((a = read_inode(file->inode, 1, address, file->offset, n)) > 0) {
            file->offset += a;
        }
        inode_unlock(file->inode);
    }
    else {
        panic("file_read");
    }
    
    return a;
}

int write_file(struct file *file, uint32_t address, int n) {

    int r, ret = 0;

    if (file->writable == 0) {
        return -1;
    }

    if (file->type == FD_PIPE) {
        pipewrite(file->pipe, address, n);
    }
    else if (file->type == FD_DEVICE) {
        if ((file->major_dev_num < 0) || (file->major_dev_num >= NUMDEV) || (!devsw[file->major_dev_num].write)) {
            return -1;
        }
        ret = devsw[file->major_dev_num].write(1, address, n);
    }
    else if (file->type == FD_INODE) {
        int max = ((MAXOPBLOCKS-1-1-2) / 2) * BUFFER_SIZE;
        int i = 0;
        
        while (i < n) {
            int n1 = n - i;
            if(n1 > max) {
                n1 = max;
            }
            begin_op();
            inode_lock(file->inode);
            if ((r = write_inode(file->inode, 1, address + i, file->offset, n1)) > 0) {
                file->offset += r;
            }
            inode_unlock(file->inode);
            end_op();

            if (r != n1) {
                break;
            }
            i += r;
        }
        ret = (i == n ? n : -1);
    }
    else {
        panic("write file");
    }

    return ret;
}

struct file *file_dup(struct file *f) {
    acquire_lock(&file_table.lock);
    if(f->ref < 1) {
        panic("filedup");
    }
    f->ref++;
    release_lock(&file_table.lock);
    return f;
}
