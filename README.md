# RISC-V-OS

A simple operating system designed to run on 32-bit RISC-V. 
Currently mainly useful for educational purposes as speed or uasbility
are not priorities. The OS is developed using QEMUs virt board, but future
versions will run on real hardware.

# UART driver
- Main input-output goes through the NS16550A UART of the virt board.
  uart.c first configures the UART and gives simple putchar, write and read
  functions for debugging rest of the OS.


# TO-DO
- Interrupt handling
- Virtual memory allocation
- Filesystem
- User-space
