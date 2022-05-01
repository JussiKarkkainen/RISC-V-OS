.globl tvec
.align 4

tvec:
    # Setup timer interrutps
    
    csrrw a0, mscratch, a0
    sw a1, 0(a0)
    sw a2, 4(a0)
    sw a3, 8(a0)
    sw a4, 12(a0)

    # add interval to mtimecmp
    lw a1, 16(a0)
    lw a2, 20(a0)
    lw a3, 0(a1)
    lw a4, 4(a1)
    add a3, a3, a2
    sltu a2, a3, a2
    add a4, a4, a2

    li a2, -1
    sw a2, 0(a1)
    sw a4, 4(a1)
    sw a3, 0(a1)

    # raise supervisor software interrupt
    li a1, 2
    csrw sip, a1

    lw a4, 12(a0)
    lw a3, 8(a0)
    lw a2, 4(a0)
    lw a1, 0(a0)
    csrrw a0, mscratch, a0

    mret
