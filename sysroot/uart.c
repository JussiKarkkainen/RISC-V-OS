#include <stdint.h>


typedef volatile struct __attribute__((packed)) {   // Tell compiler to not padd struct
    uint32_t IER;           // Interrupr enable register
    uint32_t const IIR;     // Interrupt ident. registei, read only
    uint32_t FCR;           // FIFO control register
    uint32_t LCR;           // Line control register
    uint32_t MCR;           // MODEM control register
    uint32_t LSR;           // Line status register
    uint32_t MSR;           // MODEM status register
    uint32_t SCR;           // Scratch register
} uart_regs;


static volatile uart_regs* uart = (uart_regs*)0x10000000;



typedef enum {      // return UART_OK if no errors
    UART_OK;
} uart_return;


typedef struct {
    uint8_t word_length;
    uint8_t FIFO;
    uint8_t interrupt_enable;
    uint8_t baud_rate;          // Not necessary for QEMU
} uart_init;


uart_return uart_configure(uart_init* init) {
}
  




