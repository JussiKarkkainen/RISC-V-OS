.section .text
.global _start

_start:
  la sp, stacks           # Setup stack by loading stack_top address, to sp
  li a0, 1024*4
  csrr a1, mhartid
  addi a1, a1, 1
  mul a0, a0, a1
  add sp, sp, a0
  call mstart              # Setup for supervisor mode

park:
  j park

