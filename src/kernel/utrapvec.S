.globl utrapvec
.align 4

utrapvec;
    # Traps from user mode should come here
    # utrapvec saves registers, fetches the current CPU hartid, the address
    # of utrap() and the address of the kernel page table from a trapframe.
    # It also switches satp reg to kernel page table address and calls utrap(),


