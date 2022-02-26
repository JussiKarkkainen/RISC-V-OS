riscv64-unknown-elf-gcc -Wall -Wextra -march=rv32im -mabi=ilp32 -T kernel/linker.ld -nostdlib -ffreestanding -lgcc \
kernel/boot.S kernel/kernel.c kernel/console.c kernel/uart.c libc/stdio/kprintf.c kernel/paging.c 
