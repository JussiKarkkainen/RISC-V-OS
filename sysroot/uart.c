#include <stdint.h>

#define LCR_5BIT 0x0
#define LCR_8BIT 0x3
#define FIFO_ENABLE 0x1
#define IER_ENABLE 0xf
#define ERROR_BITMASK 


typedef volatile struct __attribute__((packed)) {   // Tell compiler to not padd struct
    uint32_t BF;            // Read/Write registers
    uint32_t IER;           // Interrupr enable register
    uint32_t const IIR;     // Interrupt ident. register, read only
    uint32_t FCR;           // FIFO control register
    uint32_t LCR;           // Line control register
    uint32_t MCR;           // MODEM control register
    uint32_t LSR;           // Line status register
    uint32_t MSR;           // MODEM status register
    uint32_t SCR;           // Scratch register
} uart_regs;


static volatile uart_regs* uart = (uart_regs*)0x10000000;


typedef enum {     
    UART_OK;
    UART_FAIL;
} uart_return;


typedef struct {
    uint8_t word_length;
    uint8_t FIFO;
    uint8_t interrupt_enable;
    uint8_t baud_rate;          // Not necessary for QEMU
} uart_init;


uart_return uart_configure(uart_init* init) {
    
    // Set word length
    switch (init->word_length) {
        case 5: uart->LCR |= LCR_5BIT;

        case 8: uart->LCR |= LCR_8BIT;

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
    uart->BF = c;
}

void write_uart(char* data) {
    while (*data) {
        uart_putchar(*data);
        *data++;
    }
}


uart_return read_uart(char* c) {
    
    *c = uart->bf;
    if (uart->LSR & ERROR_BITMASK) {    // Check for errors
        uart->LSR &= ERROR_BITMASK;     // Reset error register
        return UART_FAIL;
    } 


    return UART_OK;
}


int main() {
    
    uart_init init = {
        .word_length = 8,
        .FIFO = 1,
        .interrupt_enable = 1,
        .baudrate = 9600            // This isn't used
    };

    uart_init(&init);

    uart_putchar("A");
    uart_putchar("\n");
    uart_write("Hello World\n");

    return 0;
}



