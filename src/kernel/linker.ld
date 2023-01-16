OUTPUT_ARCH("riscv");

ENTRY(_entry);

MEMORY 
    {
        ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 128M
    }

PHDRS
    {
        text PT_LOAD;
        data PT_LOAD;
        bss PT_LOAD;
    }

SECTIONS 
    {
          
          .text : ALIGN(4K) {
                  PROVIDE(text_start = .);
                  *(.text.init) *(.text .text.*);
                  . = ALIGN(0x1000);
                  _uvec = .;
                  *(uvecsec);
                  . = ALIGN(0x1000);
                  PROVIDE(text_end = .);

          } >ram AT>ram :text

          .rodata : ALIGN(4K) {
                    PROVIDE(rodata_start = .);
                    *(.rodata .rodata.*);
                    PROVIDE(rodata_end = .);
          } >ram AT>ram :text
          
          .data : ALIGN(4K) {
                  PROVIDE(data_start = .);
                  *(.sdata .sdata.*) *(.data .data.*);
                  PROVIDE(data_end = .);
          } >ram AT>ram :data
          
          .bss : ALIGN(4K) {
                 PROVIDE(bss_start = .);
                 *(.sbss .sbss.*) *(.bss .bss.*);
                 PROVIDE(bss_end = .);
          } >ram AT>ram :bss
                 
    PROVIDE(memory_start = ORIGIN(ram));
    PROVIDE(stack_top = bss_end);
    PROVIDE(stack_end = stack_top + 0x8000);
    PROVIDE(memory_end = ORIGIN(ram) + LENGTH(ram));
    PROVIDE(heap_start = stack_end);
    PROVIDE(heap_size = memory_end - heap_start);
    PROVIDE(text_size = text_end - text_start);
    PROVIDE(rodata_size = rodata_end - rodata_start);
    PROVIDE(data_size = data_end - data_start);
    PROVIDE(bss_size = bss_end - bss_start);
    PROVIDE(kernel_stack_size = 0x8000);
    }

