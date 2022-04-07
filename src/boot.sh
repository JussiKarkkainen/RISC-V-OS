riscv64-unknown-elf-gcc -Wall -Wextra -mcmodel=medany -march=rv32ima -mabi=ilp32 -T kernel/linker.ld -nostdlib -ffreestanding -lgcc \
kernel/boot.S kernel/kernel.c kernel/console.c kernel/uart.c libc/stdio/kprintf.c kernel/pmm.c kernel/mem.S \
libc/string/memset.c kernel/paging.c libc/string/memmove.c libc/string/memcpy.c kernel/locks.c \
kernel/process.c kernel/transfer.S kernel/mstart.c kernel/tvec.S kernel/trap.c kernel/plic.c kernel/ktrapvec.S \
kernel/syscall.c kernel/filesys.c kernel/sysfile.c kernel/sysproc.c kernel/file.c kernel/bufcache.c kernel/disk.c \
libc/string/strlen.c libc/string/strncmp.c libc/string/strcpy.c libc/string/memcmp.c kernel/pipes.c kernel/utrapvec.S 

qemu-system-riscv32 -nographic -smp 4 -m 128M -machine virt -bios none -kernel a.out
