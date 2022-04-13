KERNEL=src/kernel
USER=src/user
LIBCSTRING=src/libc/string
LIBCSTDIO=src/libc/stdio

OBJS = \
    $(KERNEL)/boot.o \
    $(KERNEL)/disk.o \
    $(KERNEL)/kernel.o \
    $(KERNEL)/locks.o \
    $(KERNEL)/mstart.o \
    $(KERNEL)/pipes.o \
    $(KERNEL)/transfer.o \
    $(KERNEL)/tvec.o \
    $(KERNEL)/utrapvec.o \
    $(KERNEL)/bufcache.o \
    $(KERNEL)/filesys.o \
    $(KERNEL)/ktrapvec.o \
    $(KERNEL)/paging.o \
    $(KERNEL)/pmm.o \
    $(KERNEL)/sysfile.o \
    $(KERNEL)/trap.o \
    $(KERNEL)/uart.o \
    $(KERNEL)/console.o \
    $(KERNEL)/file.o \
    $(KERNEL)/mem.o \
    $(KERNEL)/plic.o \
    $(KERNEL)/process.o \
    $(KERNEL)/syscall.o \
    $(KERNEL)/sysproc.o \


CC = riscv64-unknown-elf-gcc
CFLAGS = -Wall -Wextra
CFLAGS += -mcmodel=medany 
CFLAGS += -nostdlib -ffreestanding -lgcc

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

LD = riscv64-unknown-elf-ld
LDFLAGS = -z max-page-size=4096

$(KERNEL)/kern: $(OBJS) $(KERNEL)/linker.ld $(USER)/initcode
	$(LD) $(LDFLAGS) -T $(KERNEL)/linker.ld -o $(KERNEL)/kern $(OBJS)

$(USER)/initcode: $(USER)/initcode.S
	$(CC) $(CFLAGS) -march=rv32ima -mabi=ilp32 -nostdinc -I. -Ikernel -c $(USER)/initcode.S -o $(USER)/initcode.o
	$(LD) $(LDFLAGS) -m elf32lriscv -N -e start -Ttext 0 -o $(USER)/initcode.out $(USER)/initcode.o 


ULIB = $(USER)/malloc.o $(USER)/ulibc.o $(USER)/printf.o $(USER)/usyscall.o

$(USER)/usyscall.S: $(USER)/usyscall.pl
	perl $(USER)/usyscall.pl > $(USER)usyscall.S

$(USER)/usyscall.o: $(USER)/usyscall.S
	$(CC) $(CFLAGS) -c -o $(USER)/usyscall.o $(USER)/usyscall.S

makefs: src/makefs.c $(KERNEL)/filesys.h 
	gcc -Wall -I. -o src/makefs src/makefs.c


UPROGS = \
    $(USER)/_cat\
    $(USER)/_echo\
    $(USER)/_init\
    $(USER)/_kill\
    $(USER)/_ln\
    $(USER)/_mkdir\
    $(USER)/_rm\
    $(USER)/_sh\
    $(USER)/_wc

fs.img: src/makefs README $(UPROGS)
	src/makefs fs.img README $(UPROGS)


QEMU = qemu-system-riscv32
QEMUOPT = -machine virt -bios none -kernel $(KERNEL)/kern -m 128M -smp 4 -nographic
QEMUOPT += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0


qemu: $(KERNEL)/kern fs.img
	$(QEMU) $(QEMUOPT)

