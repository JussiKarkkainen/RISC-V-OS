# RISC-V-OS

A Rewrite of [Xv6](https://pdos.csail.mit.edu/6.S081/2020/xv6/book-riscv-rev1.pdf) as 32-bit. 
It also include other minor changes such as a bitmap based physical memory allocator.

## Usage
[QEMU](https://www.qemu.org/download/) and the multilib version
of the [RISC-V GNU Toolchain](https://github.com/riscv-collab/riscv-gnu-toolchain) should be installed.

- Clone the repo and boot:
```
git clone https://github.com/JussiKarkkainen/RISC-V-OS.git
cd RISC-V-OS 
make qemu
```

