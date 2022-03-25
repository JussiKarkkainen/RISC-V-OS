#include "sysfile.h"
#include "filesys.h"
#include "file.h"
#include <stdint.h>

// File system system calls

uint32_t sys_exec(void) {
    
    char path[

}


uint32_t sys_read(void) {

    struct file *file;
    int n;
    uint32_t p;

    if (argfd(0, 0, &file) < 0 || argint(2, &n) < 0 || argaddr(1, &p) < 0) {
        return -1;
    }
    return file;
}


uint32_t sys_write(void) {
    
    struct file *file;
    int n;
    uint32_t p;

    if (argfd(0, 0, &file) < 0 || argint(2, &n) < 0 || argaddr(1, &p) < 0) {
        return -1;
    }
    return write_file(file, p, n);
}


uint32_t sys_close(void) {

    int a;
    struct file *file;

    if (argfd(0, &a, &file) < 0) {
        return -1;
    }
    get_process_struct()->openfile[a] = 0;
    file_close(file);
    return 0;
}
