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






























