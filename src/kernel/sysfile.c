#include "sysfile.h"
#include "filesys.h"
#include "file.h"
#include <stdint.h>
#include <stddef.h>

// File system system calls

// Get the n:th system call argument as a file descriptor and return
// both the descriptor and thr file struct.
// **file is a pointer to a pointer.
int argfd(int n, int *pfd, struct file **file) {

    int fd;
    struct file *f;

    if (argint(n, &fd) < 0) {
        return -1;
    }
    if (fd < 0 || fd >= NOFILE || (file = get_process_struct()->openfile[fd]) == 0) {
        return -1;
    }
    if (pfd) {
        *pfd = fd;
    }
    if (file) {
        *file = f;
    }
    return 0;
}


uint32_t sys_exec(void) {
    
    char path[MAXPATH], *argv[MAXARG];
    int i;
    uint32_t uargv, uarg;

    if (argstr(0, path, MAXPATH) < 0 || argaddr(1, &uargv) < 0) {
        return -1;
    }
    memset(argv, 0, sizeof(argv));
    for (i = 0;; i++) {
        if (i >= NELEM(argv)) {
            goto bad;
        }
        if(fetchaddr(uargv+sizeof(uint32_t)*i, (uint32_t*)&uarg) < 0) {
            goto bad;
        }
        if(uarg == 0) {
            argv[i] = 0;
            break;
        }
        argv[i] = zalloc(1);
        if (argv[i] == 0) {
            goto bad;
        }
        if (fetchstr(arg, argv[i], PGESIZE) < 0) {
            goto bad;
        }
    }

    int ret = exec(path, argv);

    for(i = 0; i < NELEM(argv) && argv[i] != 0; i++) {
        kfree(argv[i]);
    }

    return ret;

    bad:
        for (i = 0; i < NELEM(argv) && argv[i] != 0; i++) {
            kfree(argv[i]);
        }
        return -1;
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
