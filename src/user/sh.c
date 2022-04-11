#include "user.h"


#define O_RDONLY  0x000
#define O_WRONLY  0x001
#define O_RDWR    0x002
#define O_CREATE  0x200
#define O_TRUNC   0x400

#define EXEC  1
#define REDIR 2
#define PIPE  3
#define LIST  4
#define BACK  5

#define MAXARGS 10


int main(void) {
    static char buf[100];
    int fd;

    // Ensure that three file descriptors are open.
    while ((fd = open("console", O_RDWR)) >= 0) {
        if (fd >= 3) {
            close(fd);
            break;
        }
    }

    // Read and run input commands.
    while (getcmd(buf, sizeof(buf)) >= 0) {
        if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
            // Chdir must be called by the parent, not the child.
            buf[strlen(buf)-1] = 0;  // chop \n
            if (chdir(buf+3) < 0) {
                fprintf(2, "cannot cd %s\n", buf+3);
            }
            continue;
        }
        if (fork1() == 0) {
            runcmd(parsecmd(buf));
        }
        wait(0);
    }
    exit(0);
}


void panic(char *s) {
    fprintf(2, "%s\n", s);
    exit(1);
}


int fork1(void) {
    int process_id;

    process_id = fork();
    if (process_id == -1) {
        panic("fork1, shell");
    }
    return process_id;
}


