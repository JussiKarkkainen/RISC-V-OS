#include "uart.h"

static volatile uart_regs* uart = (uart_regs *)0x10000000;


uart_return uart_configure(uart_init* init) {
    
    // Set word length
    switch (init->word_length) {
        case 5: uart->LCR |= LCR_5BIT;
            break;

        case 8: uart->LCR |= LCR_8BIT;
            break;

        default: uart->LCR |= LCR_8BIT;
            break;
    }

// set FIFO
    if (init->FIFO == 1) {
        uart->FCR |= FIFO_ENABLE;
    }
    else {
        return UART_FAIL;
    }

    // Enable interrupts
    if (init->interrupt_enable == 1) {
        uart->IER |= IER_ENABLE;
    }
    else {
        return UART_FAIL;
    }

    return UART_OK;
}
  

void uart_putchar(char c) {
    
    if (uart->LSR & (1 << 6)) {
        uart->BF = c;
    }
}

void write_uart(char* data) {
    while (*data) {
        uart_putchar(*data++);
    }
}


uart_return read_uart(char* c) {
    
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

