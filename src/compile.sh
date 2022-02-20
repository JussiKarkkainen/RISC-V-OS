riscv64-unknown-elf-gcc -Wall -Wextra -march=rv32im -mabi=ilp32 -T kernellinker.ld -nostdlib -ffreestanding -lgcc kernel/boot.S kernel/kernel.c 
