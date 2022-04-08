#include "uart.h"
#include "file.h"
#include "stdint.h"
#include "process.h"
#include "paging.h"

#define INPUT_BUF 128

struct {
    struct spinlock lock;
    char buf[INPUT_BUF];
    unsigned int read;
    unsigned int write;
    unsigned int edit;
} console;

int console_write(int user_src, uint32_t src, int n) {

    for (int i = 0; i < n; i++) {
        char c:
        if (either_copyin(&c, user_src, src+i, 1) == -1) {
            break;
        }
        uart_putc(c);
    }
    return i;
}

int console_read(int user_dst, uint32_t dst, int n) {
  
    unsigned int target;
    int c;
    char cbuf;

    target = n;
    acquire_lock(&console.lock);
    while (n > 0) {
        // wait until interrupt handler has put some
        // input into cons.buffer.
        while (console.read == console.write) {
            if (get_process_struct()->killed) {
                release_lock(&console.lock);
                return -1;
            } 
            sleep(&console.read, &console.lock);
        }

        c = console.buf[console.read++ % INPUT_BUF];

        if (c == C('D')) {  // end-of-file
            if(n < target){
                // Save ^D for next time, to make sure
                // caller gets a 0-byte result.
                cons.r--;
            }
            break;
        }

        // copy the input byte to the user-space buffer.
        cbuf = c;
        if (either_copyout(user_dst, dst, &cbuf, 1) == -1) {
            break;
        }

        dst++;
        --n;

        if (c == '\n') {
            // a whole line has arrived, return to
            // the user-level read().
            break;
        }
    }
    release_lock(&console.lock);

    return target - n;
}

int console_init(void) {

    initlock(console.lock, "console lock");

    uart_configure();
    
    devsw[CONSOLE].read = console_read;
    devsw[CONSOLE].write = console_write;

    write_uart("Type a command to continue\n");
    write_uart("$ ");
    
    while (1) {
        char c;
        if (read_uart(&c) == UART_OK) {
            uart_putc(c);
            if (c == '\r') {
                write_uart("\n");
                write_uart("$ ");
            }
        }
    }

    return 0;
}

