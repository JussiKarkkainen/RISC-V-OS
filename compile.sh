riscv64-unknown-elf-gcc -Wall -Wextra -march=rv32i -mabi=ilp32 -T linker.ld -nostdlib -ffreestanding boot.S kernel.c
