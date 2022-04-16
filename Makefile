KERNEL=src/kernel
USER=src/user
LIBCSTRING=src/libc/string
LIBCSTDIO=src/libc/stdio

ASMOBJS = \
    $(KERNEL)/boot.o \
    $(KERNEL)/ktrapvec.o \
    $(KERNEL)/mem.o \
    $(KERNEL)/transfer.o \
    $(KERNEL)/tvec.o \
    $(KERNEL)/utrapvec.o

COBJS = \
    $(KERNEL)/disk.o \
    $(KERNEL)/kernel.o \
    $(KERNEL)/locks.o \
    $(KERNEL)/mstart.o \
    $(KERNEL)/pipes.o \
    $(KERNEL)/bufcache.o \
    $(KERNEL)/filesys.o \
    $(KERNEL)/paging.o \
    $(KERNEL)/pmm.o \
    $(KERNEL)/sysfile.o \
    $(KERNEL)/trap.o \
    $(KERNEL)/uart.o \
    $(KERNEL)/console.o \
    $(KERNEL)/file.o \
    $(KERNEL)/plic.o \
    $(KERNEL)/process.o \
    $(KERNEL)/syscall.o \
    $(KERNEL)/sysproc.o \
    $(LIBCSTRING)/memcmp.o \
    $(LIBCSTRING)/memcpy.o \
    $(LIBCSTRING)/memmove.o \
    $(LIBCSTRING)/memset.o \
    $(LIBCSTRING)/strcpy.o \
    $(LIBCSTRING)/strlen.o \
    $(LIBCSTRING)/strncmp.o \
    $(LIBCSTDIO)/kprintf.o \
    $(LIBCSTDIO)/putchar.o

AS = riscv64-unknown-elf-as
ASFLAGS = -march=rv32ima -mabi=ilp32

CC = riscv64-unknown-elf-gcc
CFLAGS = -Wall -Wextra
CFLAGS += -mcmodel=medany 
CFLAGS += -nostdlib -ffreestanding -lgcc
CFLAGS += -march=rv32ima -mabi=ilp32

OBJCOPY = riscv64-unknown-elf-objcopy
OBJDUMP = riscv64-unknown-elf-objdump

# Disable PIE when possible (for Ubuntu 16.10 toolchain)
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]no-pie'),)
CFLAGS += -fno-pie -no-pie
endif
ifneq ($(shell $(CC) -dumpspecs 2>/dev/null | grep -e '[^f]nopie'),)
CFLAGS += -fno-pie -nopie
endif

LD = riscv64-unknown-elf-ld
LDFLAGS = -z max-page-size=4096
LDFLAGS += -m elf32lriscv

$(KERNEL)/kern: $(ASMOBJS) $(COBJS) $(KERNEL)/linker.ld $(USER)/initcode
	$(LD) $(LDFLAGS) -T $(KERNEL)/linker.ld -o $(KERNEL)/kern $(COBJS) $(ASMOBJS)
	$(OBJDUMP) -S $(KERNEL)/kern > $(KERNEL)/kernel.asm

$(USER)/initcode: $(USER)/initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -Ikernel -c $(USER)/initcode.S -o $(USER)/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(USER)/initcode.out $(USER)/initcode.o 
	$(OBJDUMP) -S $(USER)/initcode.out > $(USER)/initcode.asm 

ULIB = $(USER)/malloc.o $(USER)/ulibc.o $(USER)/printf.o $(USER)/usyscall.o

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^

$(USER)/usyscall.S: $(USER)/usyscall.pl
	perl $(USER)/usyscall.pl > $(USER)/usyscall.S

$(USER)/usyscall.o: $(USER)/usyscall.S
	$(CC) $(CFLAGS) -c -o $(USER)/usyscall.o $(USER)/usyscall.S

src/makefs: src/makefs.c $(KERNEL)/filesys.h 
	gcc -Wall -I. -o src/makefs src/makefs.c

.PRECIOUS: %.o

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

UOBJS = \
    $(USER)/cat.o \
    $(USER)/echo.o \
    $(USER)/initcode.o \
    $(USER)/init.o \
    $(USER)/kill.o \
    $(USER)/ln.o \
    $(USER)/malloc.o \
    $(USER)/mkdir.o \
    $(USER)/printf.o \
    $(USER)/rm.o \
    $(USER)/sh.o \
    $(USER)/ulibc.o \
    $(USER)/usyscall.o \
    $(USER)/wc.o
    
fs.img: src/makefs README.md $(UPROGS)
	src/makefs fs.img README.md $(UPROGS)

clean:
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*/*.o */*.d */*.asm */*.sym \
	$(USER)/initcode $(USER)/initcode.out $(KERNEL)/kern fs.img \
	src/makefs $(USER)/usyscall.S \
	$(UPROGS) $(COBJS) $(UOBJS) $(ASMOBJS)


QEMU = qemu-system-riscv32
QEMUOPT = -machine virt -bios none -kernel $(KERNEL)/kern -m 128M -smp 3 -nographic
QEMUOPT += -drive file=fs.img,if=none,format=raw,id=x0
QEMUOPT += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0


qemu: $(KERNEL)/kern fs.img
	$(QEMU) $(QEMUOPT)

