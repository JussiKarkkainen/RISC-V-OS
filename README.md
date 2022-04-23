# RISC-V-OS

A simple operating system based on [Unix V6](https://en.wikipedia.org/wiki/Version_6_Unix) and [MIT's Xv6](https://pdos.csail.mit.edu/6.S081/2020/xv6/book-riscv-rev1.pdf)  
developed using [QEMUs virt](https://www.qemu.org/docs/master/system/riscv/virt.html) platform. Differences compared to Xv6 are the following:
1. 32-bit instead of 64
2. TCP/IP stack
3. Bitmap based physical memory allocator


## Table of contents
- [Usage](https://github.com/JussiKarkkainen/RISC-V-OS#Usage)
- [Specification](https://github.com/JussiKarkkainen/RISC-V-OS#Specification)
- [IO](https://github.com/JussiKarkkainen/RISC-V-OS#io)
- [Memory Allocations](https://github.com/JussiKarkkainen/RISC-V-OS#memory-allocation)
- [TO-DO](https://github.com/JussiKarkkainen/RISC-V-OS#to-do)
- [Issues](https://github.com/JussiKarkkainen/RISC-V-OS#issues)

## Usage
This assumes that you have already installed [QEMU](https://www.qemu.org/download/) and the multilib version
of the [RISC-V GNU Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain).

- Clone the repo and boot:
```
git clone https://github.com/JussiKarkkainen/RISC-V-OS.git
cd RISC-V-OS 
make qemu
```

# Specification
## IO
- Main input-output goes through the NS16550A UART of the virt board.
  Even though QEMU seems to work fine without initialization ```kernel/uart.c``` first 
  configures the UART and gives write and read functions that are used to make
  a simple console.

## Memory allocation
### Physical memory allocations
- The physical memory allocator can be found in ```kernel/pmm.c```. It uses a [bitmap](https://wiki.osdev.org/Page_Frame_Allocation) 
  to keep track of allocated and free pages. 
- The Memory map can be found in MEMORYMAP.txt
### Virtual memory
- Implements the Sv32 Page-Based Virtual Memory System described in the [RISC-V Instruction set manual Volume II: Privileged Architecture](https://riscv.org/technical/specifications/) 
