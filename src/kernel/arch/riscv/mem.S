.section .rodata

.global HEAP_START 
HEAP_START: .dword heap_start

.global HEAP_SIZE
HEAP_SIZE: .dword heap_size

.global TEXT_START 
TEXT_START: .dword text_start

.global TEXT_END
TEXT_END: .dword text_end

.global TEXT_SIZE
TEXT_SIZE: .dword text_size

.global RODATA_START
RODATA_START: .dword rodata_start

.global RODATA_END
RODATA_START: .dword rodata_end

.global RODATA_SIZE
RODATA_SIZE: .dword rodata_size

.global DATA_START
DATA_START: .dword data_start

.global DATA_END
DATA_END: .dword data_end

.global DATA_SIZE
DATA_SIZE: .dword data_size

.global BSS_START
BSS_START: .dword bss_start

.global BSS_END
BSS_END: .dword bss_end

.global BSS_SIZE
BSS_SIZE: .dword bss_size

.global KERNEL_STACK_START
KERNEL_STACK_START: .dword stack_top

.global KERNEL_STACK_END
KERNEL_STACK_END: .dword stack_end

.global KERNEL_STACK_SIZE
KERNEL_STACK_SIZE: .dword kernel_stack_size
