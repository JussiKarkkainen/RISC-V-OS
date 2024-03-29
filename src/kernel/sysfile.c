#include "filesys.h"
#include "file.h"
#include <stdint.h>
#include <stddef.h>
#include "syscall.h"
#include "paging.h"
#include "../libc/include/string.h"
#include "../libc/include/stdio.h"
#include "pipes.h"
#include "process.h"

// File system system calls

// Get the n:th system call argument as a file descriptor and return
// both the descriptor and the file struct.
// **file is a pointer to a pointer.
int argfd(int n, int *pfd, struct file **file) {

    int fd;
    struct file *f;

    if (argint(n, &fd) < 0) {
        return -1;
    }
    if (fd < 0 || fd >= NUMFILE || (f = get_process_struct()->openfile[fd]) == 0) {
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
    struct process *p = get_process_struct();

    for (fd = 0; fd < NUMFILE; fd++) {
        if (p->openfile[fd] == 0) {
            p->openfile[fd] = f;
            return fd;
        }
    } 
    return -1;
}

int loadseg(uint32_t *pagetable, uint32_t va, struct inode *ip, unsigned int offset, unsigned int sz) {
    unsigned int i; 
    int n;
    uint32_t pa;

    for (i = 0; i < sz; i += PGESIZE) {
        pa = fetch_pa_addr(pagetable, va + i);
        if (pa == 0) {
            panic("loadseg: address should exist");
        }
        if (sz - i < PGESIZE) {
            n = sz - i;
        }
        else {
            n = PGESIZE;
        }
        if (read_inode(ip, 0, (uint32_t)pa, offset+i, n) != n) {
            return -1;
        }
    }
    return 0;
}

int exec(char *path, char **argv) {
    char *s, *last;
    int i, off;
    uint32_t argc, sz, sp, ustack[MAXARG], stackbase;       // These need to be added
    struct elf_header elf;
    struct inode *ip;
    struct prog_header ph;
    uint32_t *pagetable = 0, *oldpagetable;
    struct process *p = get_process_struct();
    
    begin_op();

    if ((ip = name_inode(path)) == 0) {
        end_op();
        return -1;
    }
    inode_lock(ip);
    // Check ELF header
    if (read_inode(ip, 0, (uint32_t)&elf, 0, sizeof(elf)) != sizeof(elf)) {
        goto bad;
    }
    if (elf.magic != ELF_MAGIC) {
        goto bad;
    }

    if ((pagetable = proc_pagetable(p)) == 0) {
        goto bad;
    }
       
    sz = 0; 

    // Load program into memory.
    for (i=0, off=elf.phoff; i<elf.phnum; i++, off+=sizeof(ph)) {
        if (read_inode(ip, 0, (uint32_t)&ph, off, sizeof(ph)) != sizeof(ph)) {
            goto bad;
        }
        if (ph.type != ELF_PROG_LOAD) {
            continue;
        }
        if (ph.memsz < ph.filesz) {
            goto bad;
        }
        if (ph.vaddr + ph.memsz < ph.vaddr) {
            goto bad;
        }
        if ((sz = uvmalloc(pagetable, sz, ph.vaddr + ph.memsz)) == 0) {
            goto bad;
        }
        if ((ph.vaddr % PGESIZE) != 0) {
            goto bad;
        }
        if (loadseg(pagetable, ph.vaddr, ip, ph.off, ph.filesz) < 0) {
            goto bad;
        }
    }
    
    inode_unlock(ip);
    inode_put(ip);
    end_op();
    ip = 0;

    p = get_process_struct();
    uint32_t oldsz = p->mem_size;

    // Allocate two pages at the next page boundary.
    // Use the second as the user stack.
    sz = (((sz) + PGESIZE-1) & ~(PGESIZE - 1));
    uint32_t sz1;
    if ((sz1 = uvmalloc(pagetable, sz, sz + 2*PGESIZE)) == 0) {
        goto bad;
    }

    sz = sz1;
    uvmclear(pagetable, sz-2*PGESIZE);
    sp = sz;
    stackbase = sp - PGESIZE;

    // Push argument strings, prepare rest of stack in ustack.
    for (argc = 0; argv[argc]; argc++) {
        if(argc >= MAXARG) {
            goto bad;
        }
        sp -= strlen(argv[argc]) + 1;
        sp -= sp % 16; // riscv sp must be 16-byte aligned
        if (sp < stackbase) {
            goto bad;
        }
        if (copyout(pagetable, sp, argv[argc], strlen(argv[argc]) + 1) < 0) {
            goto bad;
        }
        ustack[argc] = sp;
    }
    ustack[argc] = 0;

    // push the array of argv[] pointers.
    sp -= (argc+1) * sizeof(uint32_t);
    sp -= sp % 16;
    if (sp < stackbase) {
        goto bad;
    }
    if (copyout(pagetable, sp, (char *)ustack, (argc+1)*sizeof(uint32_t)) < 0) {
        goto bad;
    }

    // arguments to user main(argc, argv)
    // argc is returned via the system call return
    // value, which goes in a0.
    p->trapframe->a1 = sp;

    // Save program name for debugging.
    for (last=s=path; *s; s++) {
        if(*s == '/') {
            last = s+1;
        }
    }
    strcpy(p->name, last, sizeof(p->name));

    // Commit to the user image.
    oldpagetable = p->pagetable;
    p->pagetable = pagetable;
    p->mem_size = sz;
    p->trapframe->saved_pc = elf.entry;  // initial program counter = main
    p->trapframe->sp = sp; // initial stack pointer
    proc_freepagetable(oldpagetable, oldsz);
    
    return argc; // this ends up in a0, the first argument to main(argc, argv)

    bad:
        if (pagetable) {
            proc_freepagetable(pagetable, sz);
        }
    if (ip) {
        inode_unlock(ip);
        inode_put(ip);
        end_op();
    }
    return -1;
}

uint32_t sys_exec(void) {
    
    char path[MAXPATH]; 
    char *argv[MAXARG];
    int i;
    uint32_t uargv, uarg;
    
    if (argstr(0, path, MAXPATH) < 0 || argaddr(1, &uargv) < 0) {
        return -1;
    }
    memset(argv, 0, sizeof(argv));
    for (i = 0;; i++) {
        if (i >= (int)NUM_ELEM(argv)) {
            goto bad;
        }
        if(fetchaddr(uargv+sizeof(uint32_t)*i, (uint32_t*)&uarg) < 0) {
            goto bad;
        }
        if(uarg == 0) {
            argv[i] = 0;
            break;
        }
        argv[i] = (char *)kalloc();
        if (argv[i] == 0) {
            goto bad;
        }
        if (fetchstr(uarg, argv[i], PGESIZE) < 0) {
            goto bad;
        }
    }

    int ret = exec(path, argv);

    for(i = 0; i < (int)NUM_ELEM(argv) && argv[i] != 0; i++) {
        kfree((uint32_t *)argv[i]);
    }
    return ret;

    bad:
        for (i = 0; i < (int)NUM_ELEM(argv) && argv[i] != 0; i++) {
            kfree((uint32_t *)argv[i]);
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
    return read_file(file, p, n);
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
    
        if (dir_link(ip, ".", ip->inode_num) < 0 || dir_link(ip, "..", dp->inode_num) < 0) {
            panic("create dots, create()");
        }
    }

    if (dir_link(dp, name, ip->inode_num) < 0) {
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
    struct process *p = get_process_struct();
  
    begin_op();
    if(argstr(0, path, MAXPATH) < 0 || (ip = name_inode(path)) == 0) {
        end_op();
        return -1;
    }
  
    inode_lock(ip);
    if(ip->type != T_DIR) {
        inode_unlock(ip);
        inode_put(ip);
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
    if (argstr(0, path, MAXPATH) < 0 || (inode = create(path, T_DIR, 0, 0)) == 0) {
        end_op();
        return -1;
    }
    inode_unlock(inode);
    inode_put(inode);
    end_op();
    return 0; 
}

int isdirempty(struct inode *inode) {

    int offset;
    struct direntry d;

    for (offset=2*sizeof(d); offset<(int)inode->size; offset+=sizeof(d)) {
        if (read_inode(inode, 0, (uint32_t)&d, offset, sizeof(d)) != sizeof(d)) {
            panic("isdirempty, read_inode");
        }
        if (d.inode_num != 0) {
            return 0;
        }
    }
    return 1;
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

    if(strncmp(name, ".", 14) == 0 || strncmp(name, "..", 14) == 0) {
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
    inode_put(dp);

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

    if (ip->type == T_DEVICE && (ip->major_dev_num < 0 || ip->major_dev_num >= NUMDEV)){
        inode_unlock(ip);
        inode_put(ip);
        end_op();
        return -1;
    }

    if ((f = file_alloc()) == 0 || (fd = fdalloc(f)) < 0) {
        if(f) {
            file_close(f);
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

uint32_t sys_pipe(void) {

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
