#include "uart.h"

int console_init() {
    
    uart_init config = {
        .word_length = 8,
        .FIFO = 1,
        .interrupt_enable = 1
        //.baud_rate = 9600          
    };

    uart_configure(&config);

    write_uart("Type a command to continue\n");
    write_uart("$ ");
    
    while (1) {
        char c;
        if (read_uart(&c) == UART_OK) {
            uart_putchar(c);
            if (c == '\r') {
                write_uart("\n");
                write_uart("$ ");
            }
        }
    }

    return 0;
}

