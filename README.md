# RISC-V-OS

A simple operating system designed to run on 32-bit RISC-V. 
Currently mainly useful for educational purposes as speed or usability
are not priorities. The OS is developed using [QEMUs virt](https://www.qemu.org/docs/master/system/riscv/virt.html) board, but future
versions will run on real hardware.


## UART driver
- Main input-output goes through the NS16550A UART of the virt board.
  Even though QEMU seems to work fine without initialization ```uart.c``` first 
  configures the UART and gives write and read functions that are used to make
  a simple console.




## TO-DO
- Interrupt handling
- Virtual memory allocation
- Filesystem
- User-space
