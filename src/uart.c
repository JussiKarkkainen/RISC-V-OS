#include <stdint.h>

#define LCR_5BIT 0x0
#define LCR_8BIT 0x3
#define FIFO_ENABLE 0x1
#define IER_ENABLE 0xf
#define ERROR_BITMASK 0x8e 
#define DATA_MASK 0xff

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


static volatile uart_regs* uart = (uart_regs *)0x10000000;


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
  

void putchar(char c) {
    
    if (uart->LSR & (1 << 6)) {
        uart->BF = c;
    }
}

void write_uart(char* data) {
    while (*data) {
        putchar(*data++);
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


int main() {
    
    uart_init config = {
        .word_length = 8,
        .FIFO = 1,
        .interrupt_enable = 1
        //.baud_rate = 9600          
    };

    uart_configure(&config);

    putchar('A');
    putchar('\n');
    write_uart("Hello World\n");

    
    while (1) {
        char c;
        if (read_uart(&c) == UART_OK) {
            putchar(c);
            if (c == '\r') {
                write_uart("\n");
            }
        }
    }

    return 0;
}



