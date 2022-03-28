#include "filesys.h"
#include "file.h"
#include <stdint.h>
#include <stddef.h>
#include "syscall.h"

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
    if (fd < 0 || fd >= NUMFILE || (file = get_process_struct()->openfile[fd]) == 0) {
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

int fdalloc(struct file *f) {
    int fd;
    struct proc *p = myproc();

    for (fd = 0; fd < NUMFILE; fd++) {
        if (p->openfile[fd] == 0) {
            p->openfile[fd] = f;
            return fd;
        }
    } 
    return -1;
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

struct inode* create(char *path, short type, short major, short minor) {
  
    struct inode *ip, *dp;
    char name[DIRSIZE];

    if ((dp = nameiparent(path, name)) == 0) {
        return 0;
    }

    inode_lock(dp);

    if ((ip = dir_lookup(dp, name, 0)) != 0) {
        inode_unlock(dp);
        inode_put(dp);
        inode_lock(ip);
        if (type == T_FILE && (ip->type == T_FILE || ip->type == T_DEVICE)) {
            return ip;
        }
        inode_unlock(ip);
        inode_put(ip);
        return 0;
    }

    if ((ip = inode_alloc(dp->dev, type)) == 0) {
        panic("create: inode_alloc");
    }

    inode_lock(ip);
    ip->major_dev_num = major;
    ip->minor_dev_num = minor;
    ip->num_link = 1;
    inode_update(ip);

    if (type == T_DIR) { 
        dp->num_link++; 
        inode_update(dp);
    
        if (dirlink(ip, ".", ip->inode_num) < 0 || dirlink(ip, "..", dp->inode_num) < 0) {
            panic("create dots, create()");
        }
    }

    if (dirlink(dp, name, ip->inum) < 0) {
        panic("create: dirlink");
    }

    inode_unlock(dp);
    inode_put(dp);

    return ip;
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

uint32_t sys_chdir(void) {

    char path[MAXPATH];
    struct inode *ip;
    struct proc *p = get_process_struct();
  
    begin_op();
    if(argstr(0, path, MAXPATH) < 0 || (ip = namei(path)) == 0) {
        end_op();
        return -1;
    }
  
    inode_lock(ip);
    if(ip->type != T_DIR) {
        iunlockput(ip);
        end_op();
        return -1;
    }
  
    inode_unlock(ip);
    inode_put(p->cwd);
    end_op();
    p->cwd = ip;
    return 0;
}


uint32_t sys_mknod(void) {

    struct inode *inode;
    char path[MAXPATH];
    int major, minor;

    begin_op();
    if ((argstr(0, path, MAXPATH)) < 0 ||
        argint(1, &major) < 0 ||
        argint(2, &minor) < 0 ||
        (inode = create(path, T_DEVICE, major, minor)) == 0) {
        end_op();
        return -1;
    }
    inode_unlock(inode);
    inode_put(inode);
    end_op();
    return 0;
}

uint32_t sys_mkdir(void) {

    char path[MAXPATH];
    struct inode *inode;

    begin_op();
    if (argstr(0, path, MAXPATH) < 0 || (ininodee = create(path, T_DIR, 0, 0)) == 0) {
        end_op();
        return -1;
    }
    inode_unlock(inode);
    inode_put(inode);
    end_op();
    return 0; 
}

uint32_t sys_open(void) {

    char path[MAXPATH];
    int fd, omode;
    struct file *f;
    struct inode *ip;
    int n;

    if ((n = argstr(0, path, MAXPATH)) < 0 || argint(1, &omode) < 0) {
        return -1;
    }

    begin_op();

    if (omode & O_CREATE) {
        ip = create(path, T_FILE, 0, 0);
        if (ip == 0) {
            end_op();
            return -1;
        }
    } 
    else {
        if ((ip = name_inode(path)) == 0) {
            end_op();
            return -1;
        }
        inode_lock(ip);
        if (ip->type == T_DIR && omode != O_RDONLY) {
            inode_unlock(ip);
            inode_put(ip);
            end_op();
            return -1;
        }
    }

    if (ip->type == T_DEVICE && (ip->major_dev_num < 0 || ip->major_dev_num >= NDEV)) {
        inode_unlock(ip);
        inode_put(ip);
        end_op();
        return -1;
    }

    if ((f = file_alloc()) == 0 || (fd = fdalloc(f)) < 0) {
        if (f) {
            file_close(f);
        }
        inode_unlock(ip);
        inode_put(ip);
        end_op();
        return -1;
    }

    if (ip->type == T_DEVICE) {
        f->type = FD_DEVICE;
        f->major = ip->major_dev_num;
    } 
    else {
        f->type = FD_INODE;
        f->off = 0;
    }
    f->inode = ip;
    f->readable = !(omode & O_WRONLY);
    f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

    if ((omode & O_TRUNC) && ip->type == T_FILE) {
        inode_truncate(ip);
    }

    inode_unlock(ip);
    end_op();

    return fd;
}

uint32_t sys_unlink(void) {

    struct inode *ip, *dp;
    struct direntry de;
    char name[DIRSIZE], path[MAXPATH];
    unsigned int off;

    if (argstr(0, path, MAXPATH) < 0) {
        return -1;
    }

    begin_op();
    if ((dp = nameiparent(path, name)) == 0) {
        end_op();
        return -1;
    }

    inode_lock(dp);

    if(strncmp(name, ".") == 0 || strncmp(name, "..") == 0) {
        goto bad;
    }

    if ((ip = dir_lookup(dp, name, &off)) == 0) {
        goto bad;
    }
    inode_lock(ip);

    if (ip->num_link < 1) {
        panic("unlink: num_link < 1");
    }
    if (ip->type == T_DIR && !isdirempty(ip)) {
        inode_unlock(ip);
        inode_put(ip);
        goto bad;
    }

    memset(&de, 0, sizeof(de));
    if (write_inode(dp, 0, (uint32_t)&de, off, sizeof(de)) != sizeof(de)) {
        panic("unlink: write_inode");
    }
    if (ip->type == T_DIR) {
        dp->num_link--;
        inode_update(dp);
    }
    inode_unlock(dp);
    inode_put(dp;

    ip->num_link--;
    inode_update(ip);
    inode_unlock(ip);
    inode_put(ip);

    end_op();

    return 0;

    bad:
        inode_unlock(dp);
        inode_put(dp);
        end_op();
        return -1;
}


uint32_t sys_link(void) {

    char name[DIRSIZE], new[MAXPATH], old[MAXPATH];
    struct inode *dp, *ip;

    if (argstr(0, old, MAXPATH) < 0 || argstr(1, new, MAXPATH) < 0) {
        return -1;
    }

    begin_op();
    if ((ip = name_inode(old)) == 0) {
        end_op();
        return -1;
    }

    inode_lock(ip);
    if (ip->type == T_DIR) {
        inode_unlock(ip);
        end_op();
        return -1;
    }

     ip->num_link++;
    inode_update(ip);
    inode_unlock(ip);

    if ((dp = nameiparent(new, name)) == 0) {
        goto bad;
    }
    inode_lock(dp);
    if (dp->dev != ip->dev || dir_link(dp, name, ip->inode_num) < 0){
        inode_unlock(dp);
        inode_put(dp);
        goto bad;
    }
    inode_unlock(dp);
    inode_put(dp);
    inode_put(ip);

    end_op();

    return 0;

    bad:
        inode_lock(ip);
        ip->num_link--;
        inode_update(ip);
        inode_unlock(ip);
        inode_put(ip);
        end_op();
        return -1;
    }
}

uint32_t sys_fstat(void) {

    struct file *f;
    uint32_t st;

    if (argfd(0, 0, &f) < 0 || argaddr(1, &st) < 0) {
        return -1;
    }
    return file_stat(f, st);
}

uint32_t sys_open(void) {

    char path[MAXPATH];
    int fd, omode;
    struct file *f;
    struct inode *ip;
    int n;

    if ((n = argstr(0, path, MAXPATH)) < 0 || argint(1, &omode) < 0) {
        return -1;
    }

    begin_op();

    if (omode & O_CREATE) {
        ip = create(path, T_FILE, 0, 0);
        if (ip == 0) {
            end_op();
            return -1;
        }
    } 
    else {
        if ((ip = name_inode(path)) == 0) {
            end_op();
            return -1;
        }
        inode_lock(ip);
        if (ip->type == T_DIR && omode != O_RDONLY) {
            inode_unlock(ip);
            inode_put(ip);
            end_op();
            return -1;
        }
    }

    if (ip->type == T_DEVICE && (ip->major_dev_num < 0 || ip->major_dev_num >= NDEV)){
        inode_unlock(ip);
        inode_put(ip);
        end_op();
        return -1;
    }

    if ((f = file_alloc()) == 0 || (fd = fdalloc(f)) < 0) {
        if(f) {
            fileclose(f);
        }
        inode_unlock(ip);
        inode_put(ip);
        end_op();
        return -1;
    }

    if (ip->type == T_DEVICE) {
        f->type = FD_DEVICE;
        f->major_dev_num = ip->major_dev_num;
    } 
    else {
        f->type = FD_INODE;
        f->offset = 0;
    }
    f->inode = ip;
    f->readable = !(omode & O_WRONLY);
    f->writable = (omode & O_WRONLY) || (omode & O_RDWR);

    if ((omode & O_TRUNC) && ip->type == T_FILE) {
        inode_truncate(ip);
    }

    inode_unlock(ip);
    end_op();

    return fd;
}

uint32_t sys_dup(void) {

    struct file *f;
    int fd;

    if (argfd(0, 0, &f) < 0) {
        return -1;
    }
    if ((fd=fdalloc(f)) < 0) {
        return -1;
    }
    file_dup(f);
    return fd;
}

uint32_t sys_pipe(void) {

    uint32_t fdarray;
    struct file *rf, *wf;
    int fd0, fd1;
    struct process *p = get_process_struct();

    if (argaddr(0, &fdarray) < 0) {
        return -1;
    }
    if (pipealloc(&rf, &wf) < 0) {
        return -1;
    }
    fd0 = -1;
    if ((fd0 = fdalloc(rf)) < 0 || (fd1 = fdalloc(wf)) < 0) {
        if (fd0 >= 0) {
            p->openfile[fd0] = 0;
        }
        file_close(rf);
        file_close(wf);
        return -1;
    }
    if (copyout(p->pagetable, fdarray, (char*)&fd0, sizeof(fd0)) < 0 ||
        copyout(p->pagetable, fdarray+sizeof(fd0), (char *)&fd1, sizeof(fd1)) < 0) {
        p->openfile[fd0] = 0;
        p->openfile[fd1] = 0;
        file_close(rf);
        file_close(wf);
        return -1;
    }
    return 0;
}
