.globl tvec
.align 4

tvec:
    # Setup timer interrutps
    
    csrrw a0, mscratch, a0
    sd a1, 0(a0)
    sd a2, 8(a0)
    sd a3, 16(a0)

    # add interval to mtimecmp
    ld a1, 24(a0)
    ld a2, 32(a0)
    ld a3, 0(a1)
    add a3, a3, a2
    sd a3, 0(a1)

    # raise supervisor software interrupt
    li a1, 2
    csrw sip, a1

    ld a3, 16(a0)
    ld a2, 8(a0)
    ld a1, 0(a0)
    csrrw a0, mscratch, a0

    mret
