#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>

#define stat my_stat

#include "kernel/filesys.h"

#ifndef static_assert
#define static_assert(a, b) do { switch (0) case 0: case (a): ; } while (0)
#endif

#define NINODES 200
#define ROOTINO  1

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

int nbitmap = FSSIZE/(BLOCK_SIZE*8) + 1;
int ninodeblocks = NINODES / INODE_PER_BLOCK + 1;
int nlog = LOGSIZE;
int nmeta;    // Number of meta blocks (boot, sb, nlog, inode, bitmap)
int nblocks;  // Number of data blocks

int fsfd;
char zeroes[BLOCK_SIZE];
unsigned int freeinode = 1;
unsigned int freeblock;

void balloc(int);
void wsect(unsigned nt, void*);
void winode(unsigned int, struct disk_inode*);
void rinode(unsigned int inum, struct disk_inode *ip);
void rsect(unsigned int sec, void *buf);
unsigned int ialloc(unsigned short type);
void iappend(unsigned int inum, void *p, int n);
void die(const char *);

struct superblock sb;


unsigned int xint(unsigned int x) {
    unsigned int y;
    unsigned char *a = (unsigned char*)&y;
    a[0] = x;
    a[1] = x >> 8;
    a[2] = x >> 16;
    a[3] = x >> 24;
    return y;
}

unsigned short xshort(unsigned short x) {
    unsigned short y;
    unsigned char *a = (unsigned char*)&y;
    a[0] = x;
    a[1] = x >> 8;
    return y;
}

int main(int argc, char *argv[]) {

    int i, cc, fd;
    unsigned int rootino, inum, off;
    struct direntry de;
    char buf[BLOCK_SIZE];
    struct disk_inode din;

    static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

    if (argc < 2) {
        fprintf(stderr, "Usage: mkfs fs.img files...\n");
        exit(1);
    }

    printf("argc %d\n", argc);
    assert((BLOCK_SIZE % sizeof(struct disk_inode)) == 0);
    assert((BLOCK_SIZE % sizeof(struct direntry)) == 0);

    fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
    if (fsfd < 0) {
        die(argv[1]);
    }

    // 1 fs block = 1 disk sector
    nmeta = 2 + nlog + ninodeblocks + nbitmap;
    nblocks = FSSIZE - nmeta;

    sb.magic = FSMAGIC;
    sb.size = xint(FSSIZE);
    sb.num_block = xint(nblocks);
    sb.num_inodes = xint(NINODES);
    sb.num_log = xint(nlog);
    sb.log_start = xint(2);
    sb.inode_start = xint(2+nlog);
    sb.bitmap_start = xint(2+nlog+ninodeblocks);

    printf("nmeta %d (boot, super, log blocks %u inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
            nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

    freeblock = nmeta;     // the first free block that we can allocate

    for(i = 0; i < FSSIZE; i++) {
        wsect(i, zeroes);
    }

    memset(buf, 0, sizeof(buf));
    memmove(buf, &sb, sizeof(sb));
    wsect(1, buf);

    rootino = ialloc(T_DIR);
    assert(rootino == ROOTINO);

    bzero(&de, sizeof(de));
    de.inode_num = xshort(rootino);
    strcpy(de.name, ".");
    iappend(rootino, &de, sizeof(de));

    bzero(&de, sizeof(de));
    de.inode_num = xshort(rootino);
    strcpy(de.name, "..");
    iappend(rootino, &de, sizeof(de));

    for (i = 2; i < argc; i++) {
        // get rid of "user/"
        char *shortname;
        if (strncmp(argv[i], "src/user/", 9) == 0) {
            shortname = argv[i] + 9;
            
            printf("shortname, %c:\n", *shortname);
        }
        else {
            shortname = argv[i];
            printf("shortname, %c:\n", *shortname);
        }
        printf("shortname: %p\n", index(shortname, '/')); 
        assert(index(shortname, '/') == 0);

        if ((fd = open(argv[i], 0)) < 0) {
            die(argv[i]);
        }

        // Skip leading _ in name when writing to file system.
        // The binaries are named _rm, _cat, etc. to keep the
        // build operating system from trying to execute them
        // in place of system binaries like rm and cat.
        if(shortname[0] == '_') {
            shortname += 1;
        }
        inum = ialloc(T_FILE);

        bzero(&de, sizeof(de));
        de.inode_num = xshort(inum);
        strncpy(de.name, shortname, DIRSIZE);
        iappend(rootino, &de, sizeof(de));

        while ((cc = read(fd, buf, sizeof(buf))) > 0) {
            iappend(inum, buf, cc);
        }

        close(fd);
    }

    // fix size of root inode dir
    rinode(rootino, &din);
    off = xint(din.size);
    off = ((off/BLOCK_SIZE) + 1) * BLOCK_SIZE;
    din.size = xint(off);
    winode(rootino, &din);

    balloc(freeblock);

    exit(0);
}

void wsect(unsigned int sec, void *buf) {
    if (lseek(fsfd, sec * BLOCK_SIZE, 0) != sec * BLOCK_SIZE) {
        die("lseek, makefs, wsect");
    }
    if (write(fsfd, buf, BLOCK_SIZE) != BLOCK_SIZE) {
        die("write, makefs, wsect");
    }
}

void winode(unsigned int inum, struct disk_inode *ip) {
    char buf[BLOCK_SIZE];
    unsigned int bn;
    struct disk_inode *dip;

    bn = IBLOCK(inum, sb);
    rsect(bn, buf);
    dip = ((struct disk_inode*)buf) + (inum % INODE_PER_BLOCK);
    *dip = *ip;
    wsect(bn, buf);
}

void rinode(unsigned int inum, struct disk_inode *ip) {
    char buf[BLOCK_SIZE];
    unsigned int bn;
    struct disk_inode *dip;

    bn = IBLOCK(inum, sb);
    rsect(bn, buf);
    dip = ((struct disk_inode*)buf) + (inum % INODE_PER_BLOCK);
    *ip = *dip;
}

void rsect(unsigned int sec, void *buf) {
    if (lseek(fsfd, sec * BLOCK_SIZE, 0) != sec * BLOCK_SIZE) {
        die("lseek, makefs, rsect");
    }
    if (read(fsfd, buf, BLOCK_SIZE) != BLOCK_SIZE) {
        die("read, makefs, rsect");
    }
}

unsigned int ialloc(unsigned short type) {
    unsigned int inum = freeinode++;
    struct disk_inode din;

    bzero(&din, sizeof(din));
    din.type = xshort(type);
    din.num_link = xshort(1);
    din.size = xint(0);
    winode(inum, &din);
    return inum;
}

void balloc(int used) {
    unsigned char buf[BLOCK_SIZE];
    int i;

    printf("balloc: first %d blocks have been allocated\n", used);
    assert(used < BLOCK_SIZE*8);
    bzero(buf, BLOCK_SIZE);
    for(i = 0; i < used; i++) {
        buf[i/8] = buf[i/8] | (0x1 << (i%8));
    }
    printf("balloc: write bitmap block at sector %d\n", sb.bitmap_start);
    wsect(sb.bitmap_start, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void iappend(unsigned int inum, void *xp, int n) {
    char *p = (char*)xp;
    unsigned int fbn, off, n1;
    struct disk_inode din;
    char buf[BLOCK_SIZE];
    unsigned int indirect[NINDIRECT];
    unsigned int x;

    rinode(inum, &din);
    off = xint(din.size);
    // printf("append inum %d at off %d sz %d\n", inum, off, n);
    while (n > 0) {
        fbn = off / BLOCK_SIZE;
        assert(fbn < MAXFILE);
        if (fbn < NDIRECT) {
            if(xint(din.addresses[fbn]) == 0) {
                din.addresses[fbn] = xint(freeblock++);
            }
            x = xint(din.addresses[fbn]);
        } 
        else {
            if(xint(din.addresses[NDIRECT]) == 0) {
                din.addresses[NDIRECT] = xint(freeblock++);
            }
            rsect(xint(din.addresses[NDIRECT]), (char*)indirect);
        if (indirect[fbn - NDIRECT] == 0) {
            indirect[fbn - NDIRECT] = xint(freeblock++);
            wsect(xint(din.addresses[NDIRECT]), (char*)indirect);
        }
        x = xint(indirect[fbn-NDIRECT]);
        }
        n1 = min(n, (fbn + 1) * BLOCK_SIZE - off);
        rsect(x, buf);
        bcopy(p, buf + off - (fbn * BLOCK_SIZE), n1);
        wsect(x, buf);
        n -= n1;
        off += n1;
        p += n1;
    }
    din.size = xint(off);
    winode(inum, &din);
}

void die(const char *s) {
    perror(s);
    exit(1);
}
