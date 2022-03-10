riscv64-unknown-elf-gcc -Wall -Wextra -march=rv32ima -mabi=ilp32 -T kernel/linker.ld -nostdlib -ffreestanding -lgcc \
kernel/boot.S kernel/kernel.c kernel/console.c kernel/uart.c libc/stdio/kprintf.c kernel/pmm.c kernel/mem.S \
libc/string/memset.c libc/stdio/panic.c kernel/paging.c libc/string/memmove.c libc/string/memcpy.c kernel/locks.c \
kernel/process.c kernel/transfer.S kernel/mstart.c kernel/tvec.S kernel/trap.c

qemu-system-riscv32 -nographic -smp 4 -m 128M -machine virt -bios none -kernel a.out
