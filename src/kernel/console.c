#include "uart.h"
#include "file.h"
#include "stdint.h"
#include "process.h"
#include "paging.h"

#define C(x)  ((x)-'@')  // Control-x
#define INPUT_BUF 128
#define BACKSPACE 0x100

struct {
    struct spinlock lock;
    char buf[INPUT_BUF];
    unsigned int read;
    unsigned int write;
    unsigned int edit;
} console;

void console_putc(int c) {
    
    if (c == BACKSPACE) {
        uartputc_sync('\b'); 
        uartputc_sync(' '); 
        uartputc_sync('\b');
    }
    else {
        uartputc_sync(c);
    }
}

int console_write(int user_src, uint32_t src, int n) {
    int i;
    for (i = 0; i < n; i++) {
        char c;
        if (either_copyin(&c, user_src, src+i, 1) == -1) {
            break;
        }
        uart_putc(c);
    }
    return i;
}

int console_read(int user_dst, uint32_t dst, int n) {
  
    int target;
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
                console.read--;
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

void console_intr(int c) {
  
    acquire_lock(&console.lock);

    switch(c) {
        
        case C('P'):  // Print process list.
            procdump();
            break;
        
        case C('U'):  // Kill line.
            while(console.edit != console.write &&
                  console.buf[(console.edit-1) % INPUT_BUF] != '\n') {
                
                console.edit--;
                console_putc(BACKSPACE);
            }
            break;
  
        case '\x7f':
            if(console.edit != console.write) {
                console.edit--;
                console_putc(BACKSPACE);
            }
            break;
  
        default:
            if (c != 0 && console.edit-console.read < INPUT_BUF) {
                c = (c == '\r') ? '\n' : c;

                // echo back to the user.
                console_putc(c);

                // store for consumption by consoleread().
                console.buf[console.edit++ % INPUT_BUF] = c;

                if (c == '\n' || c == C('D') || console.edit == console.read+INPUT_BUF) {
                    // wake up consoleread() if a whole line (or end-of-file)
                    // has arrived.
                    console.write = console.edit;
                    wakeup(&console.read);
                }
            }
            break;
    }
  
    release_lock(&console.lock);
}

int console_init(void) {

    initlock(&console.lock, "console lock");

    uart_configure();
    
    devsw[CONSOLE].read = console_read;
    devsw[CONSOLE].write = console_write;
}
