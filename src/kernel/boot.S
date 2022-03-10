.option norvc

.section .text.init

.globl _start
_start:

.option push
.option norelax
  la gp, global_pointer
.option pop

  la t5, bss_start        # Load bss_start and end addresses to tmp regs and clear bss
  la t6, bss_end          # bss_start and end are defined in linker script.
  bgeu t5, t6, bss_clear


bss_clear:
  sw zero, (t5)           # Store zero into the memory address at t5
  addi t5, t5, 8          # Add 8 to t5 to move to the next memory segment
  bltu t5, t6, bss_clear  # Repeat until bss is clear


stack_setup:
  la sp, stacks        # Setup stack by loading stack_top address, to sp
  call mstart              # Setup for supervisor mode
  li a0, 1024*4
  csrr a1, mhartid
  addi a1, a1, 1
  mul a0, a0, a1
  add sp, sp, a0

park:
  wfi
  j park

