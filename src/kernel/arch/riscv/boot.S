.option norvc

.section .text.init

.global _start
_start:

  csrr t0, mhartid        # read hart id into temporary register
  bnez t0, park           # Check if hart id is zero, if not send to park
  

  csrw satp, zero         # Write zero to SATP register, used later to control MMU

.option push
.option norelax
  la gp, global_pointer
.option pop

  la t5, bss_start        # Load bss_start and end addresses to tmp regs and clear bss
  la t6, bss_end          # bss_start and end are defined in linker script.
  bgeu t5, t6, bss_clear


stack_setup:
  la sp, stack_end        # Setup stack by loading stack_top address, to sp
  li t0, (0b11 << 11) | (1 << 7) | (1 <<3)
  csrw mstatus, t0
  li t3, (1 << 3) | (1 << 7) | (1 << 11)
  csrw mie, t3
  call enter              # Jump to C code



bss_clear:
  sw zero, (t5)           # Store zero into the memory address at t5
  addi t5, t5, 8          # Add 8 to t5 to move to the next memory segment
  bltu t5, t6, bss_clear  # Repeat until bss is clear
  j stack_setup           


park:
  wfi
  j park

