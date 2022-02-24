.globl utrapvec
.align 4

utrapvec;
    # Traps from user mode should come here
    # utrapvec saves registers, fetches the current CPU hartid, the address
    # of utrap() and the address of the kernel page table from a trapframe.
    # It also switches satp reg to kernel page table address and calls utrap(),

    csrrw a0, sscratch, a0
    
    // Saving registers in the trapframe
    sd ra, 40(a0)
    sd sp, 48(a0)
    sd gp, 56(a0)
    sd tp, 64(a0)
    sd t0, 72(a0)
    sd t1, 80(a0)
    sd t2, 88(a0)
    sd s0, 96(a0)
    sd s1, 104(a0)
    sd a1, 120(a0)
    sd a2, 128(a0)
    sd a3, 136(a0)
    sd a4, 144(a0)
    sd a5, 152(a0)
    sd a6, 160(a0)
    sd a7, 168(a0)
    sd s2, 176(a0)
    sd s3, 184(a0)
    sd s4, 192(a0)
    sd s5, 200(a0)
    sd s6, 208(a0)
    sd s7, 216(a0)
    sd s8, 224(a0)
    sd s9, 232(a0)
    sd s10, 240(a0)
    sd s11, 248(a0)
    sd t3, 256(a0)
    sd t4, 264(a0)
    sd t5, 272(a0)
    sd t6, 280(a0)

    # save a0 to trapframe->a0
    csrr t0, sscratch
    sd t0, 112(a0)

    # Load kernel_sp from trapframe->kernel_sp
    ld sp, 8(a0)

    # load hart_id 
    ld tp, 32(a0)

    # load address of utrap()
    ld t0, 16(a0)

    # restore kernel pagetable
    ld t1, 0(a0)
    csrw satp, t1
    sfence.vma zero, zero

    # jump to utrap()
    jr t0

