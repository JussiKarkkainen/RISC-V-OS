#include "uart.h"

static volatile uart_regs* uart = (uart_regs *)0x10000000;


uart_return uart_configure(void) {
    // disable interrupts for configure
    uart->IER &= 0x00;

    // Set baudrate
    uart->LCR |= LCR_DLAB;
    // Normally BF is used for sending data, but not when the DLAB bit is set
    uart->BF |= 0x3;
    uart->IER |= 0x0;
    uart->LCR &= 0x3f;

    // Set word length
    uart->LCR |= LCR_8BIT;
    // Enable parity bit
    uart->LCR |= PARITY_ENABLE;

    // set FIFO
    uart->FCR |= FIFO_RESET;
    uart->FCR |= FIFO_ENABLE;

    // enable interrupts
    uart->IER |= IER_ENABLE; 

    return UART_OK;
}


void uart_putchar(char c) {
    if (uart->LSR & (1 << 6)) {
        uart->BF = c;
    }
}

void write_uart(char *data) {
    while (*data) {
        uart_putchar(*data++);
    }
}


uart_return read_uart(char *c) {
    
    if ((uart->LSR & 1) == 0) {
        return UART_NODATA;
    }

    *c = uart->BF & DATA_MASK;
    
     
    if (uart->LSR & ERROR_BITMASK) {    // Check for errors
        uart->LSR &= ~ERROR_BITMASK;     // Reset error register
        return UART_FAIL;
    } 
     
    
    return UART_OK;
}

void uart_intr(void) {
}
