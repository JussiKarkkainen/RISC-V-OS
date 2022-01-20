CC=riscv64-unknown-elf-gcc

CFLAGS:= -Wall -Wextra -march=rv32i -mabi=ilp32 -nostdlib -ffreestanding

ARCHDIR=arch/riscv

OBJS=\
	kernel.o \
	uart.o \
	tty.o \
    boot.o \


.PHONY: all clean install install-headers install kernel


all:
	kernel

kernel: $(OBJS) $(ACRHDIR)/linker.ld
    $(CC) -T $(ARCHDIR)/linker.ld -o $@ $(CFLAGS)


clean:
	rm -f kernel
	rm -f $(OBJS)

install:
	install-headers install-kernel


 install-headers:

install-kernel: 
	kernel




    
