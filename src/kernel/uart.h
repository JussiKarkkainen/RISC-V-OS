#ifndef UART_H
#define UART_H

#include <stdint.h>

#define LCR_DLAB (1 << 7)
#define LCR_8BIT 0x3
#define FIFO_ENABLE (1 << 0)
#define FIFO_RESET (3 << 1)
#define IER_ENABLE 0x3
#define ERROR_BITMASK 0x8e 
#define DATA_MASK 0xff
#define PARITY_ENABLE (1 << 3)
#define LCR_THRE (1 << 5)

typedef volatile struct __attribute__((packed)) {   // Tell compiler to not padd struct
    uint8_t BF;            // Read/Write registers
    uint8_t IER;           // Interrupt enable register
    uint8_t FCR;           // FIFO control register
    uint8_t LCR;           // Line control register
    uint8_t MCR;           // MODEM control register
    uint8_t LSR;           // Line status register
    uint8_t MSR;           // MODEM status register
    uint8_t SCR;           // Scratch register
} uart_regs;

typedef enum {     
    UART_OK = 0,
    UART_FAIL,
    UART_NODATA
} uart_return;


typedef struct {
    uint8_t word_length;
    uint8_t FIFO;
    uint8_t interrupt_enable;
    // uint8_t baud_rate;          // Not necessary for QEMU
} uart_init;

// Uart control functions
uart_return uart_configure(void);
void uart_putchar(char c);
void write_uart(char* data);
uart_return read_uart(char* c);
void uart_putc(int c);

// Console init function
int console_init(void);

#endif
