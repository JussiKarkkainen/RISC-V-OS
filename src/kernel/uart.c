#include "uart.h"
#include "locks.h"
#include "stdint.h"
#include "process.h"

static volatile uart_regs* uart = (uart_regs *)0x10000000;

struct spinlock uart_tx_lock;

char uart_tx_buf[TX_BUFSIZE];

uint32_t uart_tx_w;
uint32_t uart_tx_r;

uart_return uart_configure(void) {
    // disable interrupts for configure
    uart->IER &= 0x00;

    // Set baudrate
    uart->LCR |= LCR_DLAB;
    // Normally BF is used for sending data, but not when the DLAB bit is set
    uart->BF |= 0x3;
    uart->IER |= 0x0;
    uart->LCR &= 0x3f;
    initlock(&uart_tx_lock, "uart");

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

extern volatile int panicked;

void uartputc_sync(int c) {
    
    lock_intr_disable();

    if (panicked) {
        while(1) {
            ;
        }
    }
    while ((uart->LSR & LSR_TX_IDLE) == 0) {
        ;
    }
    uart->BF = c;
    lock_intr_enable();
}

void uart_putc(int c) {
  
    acquire_lock(&uart_tx_lock);

    if (panicked) {
        while (1) {
            ;
        }
    }

    while (1) {
        if(uart_tx_w == uart_tx_r + TX_BUFSIZE) {
            // buffer is full.
            // wait for uartstart() to open up space in the buffer.
            sleep(&uart_tx_r, &uart_tx_lock);
        } 
        else {
            uart_tx_buf[uart_tx_w % TX_BUFSIZE] = c;
            uart_tx_w += 1;
            uart_start();
            release_lock(&uart_tx_lock);
            return;
        }
    }
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

void uart_start(void) {

    while (1) {
        if (uart_tx_w == uart_tx_r) {
            return;
        }

        if ((uart->LSR & LSR_TX_IDLE) == 0) {
            return;
        }

        int c = uart_tx_buf[uart_tx_r % TX_BUFSIZE];
        uart_tx_r += 1;

        wakeup(&uart_tx_r);

        uart->BF = c;
    }
}

void uart_intr(void) {

    while (1) {
        int c = read_uart();
        if (c == -1) {
            break;
        }
        console_intr(c);
    }

    acquire_lock(&uart_tx_loxk);
    uart_start();
    release_lock(&uart_tx_lock);
}

