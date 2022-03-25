#include "sysfile.h"
#include "filesys.h"
#include <stdint.h>

// File system system calls


uint32_t sys_exec(void) {
    
    char path[

}


uint32_t sys_read(void) {
    }


uint32_t sys_write(void) {
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
