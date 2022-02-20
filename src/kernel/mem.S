.section .rodata

.globl HEAP_START 
HEAP_START: .dword heap_start

.globl HEAP_SIZE
HEAP_SIZE: .dword heap_size

.globl TEXT_START 
TEXT_START: .dword text_start

.globl TEXT_END
TEXT_END: .dword text_end

.globl TEXT_SIZE
TEXT_SIZE: .dword text_size

.globl RODATA_START
RODATA_START: .dword rodata_start

.globl RODATA_END
RODATA_START: .dword rodata_end

.globl RODATA_SIZE
RODATA_SIZE: .dword rodata_size

.globl DATA_START
DATA_START: .dword data_start

.globl DATA_END
DATA_END: .dword data_end

.globl DATA_SIZE
DATA_SIZE: .dword data_size

.globl BSS_START
BSS_START: .dword bss_start

.globl BSS_END
BSS_END: .dword bss_end

.globl BSS_SIZE
BSS_SIZE: .dword bss_size

.globl KERNEL_STACK_START
KERNEL_STACK_START: .dword stack_top

.globl KERNEL_STACK_END
KERNEL_STACK_END: .dword stack_end

.globl KERNEL_STACK_SIZE
KERNEL_STACK_SIZE: .dword kernel_stack_size
