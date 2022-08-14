KERNEL=src/kernel
USER=src/user
LIBCSTRING=src/libc/string
LIBCSTDIO=src/libc/stdio
LIBC=src/libc
NET=src/net

OBJS = \
    $(KERNEL)/boot.o \
    $(KERNEL)/mstart.o \
    $(KERNEL)/console.o \
    $(LIBCSTDIO)/kprintf.o \
    $(KERNEL)/kalloc.o \
    $(KERNEL)/uart.o \
    $(KERNEL)/locks.o \
    $(LIBCSTRING)/memset.o \
    $(LIBCSTRING)/memcmp.o \
    $(LIBCSTRING)/memcpy.o \
    $(LIBCSTRING)/memmove.o \
    $(LIBCSTRING)/strcpy.o \
    $(LIBCSTRING)/strlen.o \
    $(LIBCSTRING)/strncmp.o \
    $(KERNEL)/kernel.o \
    $(KERNEL)/pipes.o \
    $(KERNEL)/virtio_disk.o \
    $(KERNEL)/bufcache.o \
    $(KERNEL)/filesys.o \
    $(KERNEL)/paging.o \
    $(KERNEL)/sysfile.o \
    $(KERNEL)/trap.o \
    $(KERNEL)/ktrapvec.o \
    $(KERNEL)/tvec.o \
    $(KERNEL)/file.o \
    $(KERNEL)/plic.o \
    $(KERNEL)/utrapvec.o \
    $(KERNEL)/transfer32.o \
    $(KERNEL)/process.o \
    $(KERNEL)/syscall.o \
    $(KERNEL)/sysproc.o \
    $(LIBCSTDIO)/putchar.o \
    $(LIBCSTDIO)/asmprint.o \
    $(NET)/sysnet.o \
    $(NET)/socket.o \
    $(NET)/tcp.o \
    $(NET)/udp.o \
    $(LIBC)/isdigit.o \
    $(LIBC)/isascii.o \
    $(LIBC)/isspace.o \
    $(LIBC)/isxdigit.o \
    $(LIBC)/islower.o

#   $(NET)/virtio-net.o \
#   $(NET)/pcie.o

    #$(KERNEL)/pmm.o \
    #$(KERNEL)/disk.o \

AS = riscv64-unknown-elf-as
ASFLAGS = -march=rv32ima -mabi=ilp32


CC = riscv64-unknown-elf-gcc
CFLAGS = -Wall -Werror -O -fno-omit-frame-pointer -ggdb
CFLAGS += -MD
CFLAGS += -mcmodel=medany 
CFLAGS += -nostdlib -ffreestanding -fno-common -mno-relax
CFLAGS += -I.
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
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

$(KERNEL)/kern: $(OBJS) $(KERNEL)/kernel.ld $(USER)/initcode
	$(LD) $(LDFLAGS) -T $(KERNEL)/kernel.ld -o $(KERNEL)/kern $(OBJS) 
	$(OBJCOPY) -S -O binary $(USER)/initcode.out $(USER)/initcode
	$(OBJDUMP) -S $(KERNEL)/kern > $(KERNEL)/kernel.asm

$(USER)/initcode: $(USER)/initcode.S
	$(CC) $(CFLAGS) -nostdinc -I. -Ikernel -c $(USER)/initcode.S -o $(USER)/initcode.o
	$(LD) $(LDFLAGS) -N -e start -Ttext 0 -o $(USER)/initcode.out $(USER)/initcode.o 
	$(OBJCOPY) -S -O binary $(USER)/initcode.out $(USER)/initcode
	$(OBJDUMP) -S $(USER)/initcode.out > $(USER)/initcode.asm 

tags: $(OBJS) _init
	etags *.S *.c

ULIB = $(USER)/inet_addr.o $(USER)/malloc.o $(USER)/ulibc.o $(USER)/printf.o $(USER)/usyscall.o

_%: %.o $(ULIB)
	$(LD) $(LDFLAGS) -N -e main -Ttext 0 -o $@ $^
	$(OBJDUMP) -S $@ > $*.asm
	$(OBJDUMP) -t $@ | sed '1,/SYMBOL TABLE/d; s/ .* / /; /^$$/d' > $*.sym

$(USER)/usyscall.S: $(USER)/usyscall.pl
	perl $(USER)/usyscall.pl > $(USER)/usyscall.S

$(USER)/usyscall.o: $(USER)/usyscall.S
	$(CC) $(CFLAGS) -c -o $(USER)/usyscall.o $(USER)/usyscall.S

src/makefs: src/makefs.c $(KERNEL)/filesys.h 
	gcc -Werror -Wall -I. -o src/makefs src/makefs.c

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
    $(USER)/_wc\
    $(USER)/_ls\
    $(USER)/_getwebsite
	
fs.img: src/makefs README.md $(UPROGS)
	src/makefs fs.img README.md $(UPROGS)

-include src/kernel/*.d src/user/*.d

clean:
	rm -f *.tex *.dvi *.idx *.aux *.log *.ind *.ilg \
	*/*.o */*.d */*.asm */*.sym \
	$(USER)/initcode $(USER)/initcode.out $(KERNEL)/kern fs.img \
	src/makefs $(USER)/usyscall.S \
	$(UPROGS) 

GDBPORT = $(shell expr `id -u` % 5000 + 25000)
QEMUGDB = $(shell if $(QEMU) -help | grep -q '^-gdb'; \
	then echo "-gdb tcp::$(GDBPORT)"; \
	else echo "-s -p $(GDBPORT)"; fi)

QEMU = qemu-system-riscv32
QEMUOPT = -machine virt -bios none -kernel $(KERNEL)/kern -m 1024M -smp 3 -nographic
QEMUOPT += -drive file=fs.img,if=none,format=raw,id=x0
QEMUOPT += -device virtio-blk-device,drive=x0,bus=virtio-mmio-bus.0
QEMUOPT += -netdev user,id=network0 -device virtio-net-pci,netdev=network0
# QEMUOPT += -machine dumpdtb=/tmp/dump.dtb

qemu: $(KERNEL)/kern fs.img
	$(QEMU) $(QEMUOPT)

.gdbinit: .gdbinit.tmpl-riscv
	sed "s/:1234/:$(GDBPORT)/" < $^ > $@

qemu-gdb: $(KERNEL)/kern .gdbinit fs.img
	@echo "*** Now run 'gdb' in another window." 1>&2
	$(QEMU) $(QEMUOPTS) -S $(QEMUGDB)

