

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
    sw ra, 40(a0)
    sw sp, 48(a0)
    sw gp, 56(a0)
    sw tp, 64(a0)
    sw t0, 72(a0)
    sw t1, 80(a0)
    sw t2, 88(a0)
    sw s0, 96(a0)
    sw s1, 104(a0)
    sw a1, 120(a0)
    sw a2, 128(a0)
    sw a3, 136(a0)
    sw a4, 144(a0)
    sw a5, 152(a0)
    sw a6, 160(a0)
    sw a7, 168(a0)
    sw s2, 176(a0)
    sw s3, 184(a0)
    sw s4, 192(a0)
    sw s5, 200(a0)
    sw s6, 208(a0)
    sw s7, 216(a0)
    sw s8, 224(a0)
    sw s9, 232(a0)
    sw s10, 240(a0)
    sw s11, 248(a0)
    sw t3, 256(a0)
    sw t4, 264(a0)
    sw t5, 272(a0)
    sw t6, 280(a0)

    # save a0 to trapframe->a0
    csrr t0, sscratch
    sw t0, 112(a0)

    # Load kernel_sp from trapframe->kernel_sp
    lw sp, 8(a0)

    # load hart_id 
    lw tp, 32(a0)

    # load address of utrap()
    lw t0, 16(a0)

    # restore kernel pagetable
    lw t1, 0(a0)
    csrw satp, t1
    sfence.vma zero, zero

    # jump to utrap()
    jr t0


# utrapreturn is called from utrapret() in trap.c
# Its job is to switch from kernel to user mode

utrapreturn:
    
    # Switch to a user pagetable
    csrw satp, a1
    sfence.vma zero, zero

    # Put a0 to sscratch so we can swap it with trapframe
    lw t0, 112(a0)
    csrw sscratch, t0

    # restore registers
    lw ra, 40(a0)
    lw sp, 48(a0)
    lw gp, 56(a0)
    lw tp, 64(a0)
    lw t0, 72(a0)
    lw t1, 80(a0)
    lw t2, 88(a0)
    lw s0, 96(a0)
    lw s1, 104(a0)
    lw a1, 120(a0)
    lw a2, 128(a0)
    lw a3, 136(a0)
    lw a4, 144(a0)
    lw a5, 152(a0)
    lw a6, 160(a0)
    lw a7, 168(a0)
    lw s2, 176(a0)
    lw s3, 184(a0)
    lw s4, 192(a0)
    lw s5, 200(a0)
    lw s6, 208(a0)
    lw s7, 216(a0)
    lw s8, 224(a0)
    lw s9, 232(a0)
    lw s10, 240(a0)
    lw s11, 248(a0)
    lw t3, 256(a0)
    lw t4, 264(a0)
    lw t5, 272(a0)
    lw t6, 280(a0)

    # Restore a0
    csrrw a0, sscratch, a0

    # Return to user mode
    sret

