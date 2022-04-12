CC=riscv64-unknown-elf-gcc

CFLAGS= -Wall -Wextra
CFLAGS+= -nostdlib -ffreestanding -lgcc
CFLAGS+= -march=rv32im -mabi=ilp32 -mcmodel=medlow

ARCHDIR=kernel/

OBJS=\
    $(ARCHDIR)boot.o \
    $(ARCHDIR)kernel.o \
	$(ARCHDIR)uart.o \
	$(ARCHDIR)console.o \



all: kernel 

kernel: $(OBJS) $(ARCHDIR)linker.ld
	$(CC) $(CFLAGS) -o kern $(OBJS) -T $(ARCHDIR)linker.ld

