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

## Memory allocation
### Physical memory allocations
- The physical memory allocator can be found in ```kernel/pmm.c```. It uses a [bitmap](https://wiki.osdev.org/Page_Frame_Allocation) 
  to keep track of allocated and free pages. 

## Virtual memory
- Implements the Sv32 Page-Based Virtual Memory System described in the [RISC-V Instruction set manual Volume II: Priviliged Architecture](https://riscv.org/technical/specifications/) 

## TO-DO
- Interrupt handling
- Memory allocation and programming the MMU
- Filesystem
- User-space
### Issues
- Fix infinite loops, that are caused by writing to uart. Can be avoided by calling console_init(), but isn't ideal.

