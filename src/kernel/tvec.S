.globl tvec
.align 4

tvec:
    # Setup timer interrutps
    
    csrrw a0, mscratch, a0
    sw a1, 0(a0)
    sw a2, 8(a0)
    sw a3, 16(a0)

    # add interval to mtimecmp
    lw a1, 24(a0)
    lw a2, 32(a0)
    lw a3, 0(a1)
    add a3, a3, a2
    sw a3, 0(a1)

    # raise supervisor software interrupt
    li a1, 2
    csrw sip, a1

    lw a3, 16(a0)
    lw a2, 8(a0)
    lw a1, 0(a0)
    csrrw a0, mscratch, a0

    mret
