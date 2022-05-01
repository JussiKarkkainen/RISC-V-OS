.section .text
.global _start



_start:
  la t5, bss_start        # Load bss_start and end addresses to tmp regs and clear bss
  la t6, bss_end          # bss_start and end are defined in linker script.
  bgeu t5, t6, bss_clear

bss_clear:
  sw zero, (t5)
  addi t5, t5, 4
  bltu t5, t6, stack

stack:  
  la sp, stacks           # Setup stack by loading stack_top address, to sp
  li a0, 1024*4
  csrr a1, mhartid
  addi a1, a1, 1
  mul a0, a0, a1
  add sp, sp, a0
  call mstart              # Setup for supervisor mode

park:
  j park

