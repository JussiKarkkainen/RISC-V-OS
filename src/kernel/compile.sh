riscv64-unknown-elf-gcc -Wall -Wextra -march=rv32i -mabi=ilp32 -T arch/riscv/linker.ld -nostdlib -ffreestanding arch/riscv/boot.S kernel.c arch/riscv/uart.c
