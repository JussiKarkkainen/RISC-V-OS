riscv64-unknown-elf-gcc -march=rv32imac -mabi=ilp32 -T linker.ld -nostdlib -fno-builtin boot.S kernel.c
