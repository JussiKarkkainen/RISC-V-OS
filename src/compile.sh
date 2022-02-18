riscv64-unknown-elf-gcc -Wall -Wextra -march=rv32im -mabi=ilp32 -T kernel/arch/riscv/linker.ld -nostdlib -ffreestanding -lgcc kernel/arch/riscv/boot.S kernel/kernel.c 
