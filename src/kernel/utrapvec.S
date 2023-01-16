
.section uvecsec
.globl uvec
uvec:
.align 4
.globl utrapvec

utrapvec:
    # Traps from user mode should come here
    # utrapvec saves registers, fetches the current CPU hartid, the address
    # of utrap() and the address of the kernel page table from a trapframe.
    # It also switches satp reg to kernel page table address and calls utrap(),

    csrrw a0, sscratch, a0
    
    // Saving registers in the trapframe
    sw ra, 20(a0)
    sw sp, 24(a0)
    sw gp, 28(a0)
    sw tp, 32(a0)
    sw t0, 36(a0)
    sw t1, 40(a0)
    sw t2, 44(a0)
    sw s0, 48(a0)
    sw s1, 52(a0)
    sw a1, 60(a0)
    sw a2, 64(a0)
    sw a3, 68(a0)
    sw a4, 72(a0)
    sw a5, 76(a0)
    sw a6, 80(a0)
    sw a7, 84(a0)
    sw s2, 88(a0)
    sw s3, 92(a0)
    sw s4, 96(a0)
    sw s5, 100(a0)
    sw s6, 104(a0)
    sw s7, 108(a0)
    sw s8, 112(a0)
    sw s9, 116(a0)
    sw s10, 120(a0)
    sw s11, 124(a0)
    sw t3, 128(a0)
    sw t4, 132(a0)
    sw t5, 136(a0)
    sw t6, 140(a0)

    # save a0 to trapframe->a0
    csrr t0, sscratch
    sw t0, 56(a0)

    # Load kernel_sp from trapframe->kernel_sp
    lw sp, 4(a0)

    # load hart_id 
    lw tp, 16(a0)

    # load address of utrap()
    lw t0, 8(a0)

    # restore kernel pagetable
    lw t1, 0(a0)
    csrw satp, t1
    sfence.vma zero, zero

    # jump to utrap()
    jr t0


# utrapreturn is called from utrapret() in trap.c
# Its job is to switch from kernel to user mode

.globl utrapreturn
utrapreturn:
    
    # Switch to a user pagetable
    csrw satp, a1
    sfence.vma zero, zero

    # Put a0 to sscratch so we can swap it with trapframe
    lw t0, 56(a0)
    csrw sscratch, t0

    # restore registers
    lw ra, 20(a0)
    lw sp, 24(a0)
    lw gp, 28(a0)
    lw tp, 32(a0)
    lw t0, 36(a0)
    lw t1, 40(a0)
    lw t2, 44(a0)
    lw s0, 48(a0)
    lw s1, 52(a0)
    lw a1, 60(a0)
    lw a2, 64(a0)
    lw a3, 68(a0)
    lw a4, 72(a0)
    lw a5, 76(a0)
    lw a6, 80(a0)
    lw a7, 84(a0)
    lw s2, 88(a0)
    lw s3, 92(a0)
    lw s4, 96(a0)
    lw s5, 100(a0)
    lw s6, 104(a0)
    lw s7, 108(a0)
    lw s8, 112(a0)
    lw s9, 116(a0)
    lw s10, 120(a0)
    lw s11, 124(a0)
    lw t3, 128(a0)
    lw t4, 132(a0)
    lw t5, 136(a0)
    lw t6, 140(a0)

    # Restore a0
    csrrw a0, sscratch, a0

    # Return to user mode
    sret

