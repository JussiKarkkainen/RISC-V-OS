#include "uart.c"

int console_init(void) {
    
    uart_configure();

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

