OUTPUT_ARCH("riscv");

ENTRY(_start);

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
          
          .text : {
                  PROVIDE(text_start = .);
                  *(.text.init) *(.text .text.*);
                  PROVIDE(text_end = .);
          } >ram AT>ram :text

          PROVIDE(global_pointer = .);
          
          .rodata : {

                    PROVIDE(rodata_start = .);
                    *(.rodata .rodata.*);
                    PROVIDE(rodata_end = .);
          } >ram AT>ram :text
          
          .data : {
                  . = ALIGN(4096);
                  PROVIDE(data_start = .);
                  *(.sdata .sdata.*) *(.data .data.*);
                  PROVIDE(data_end = .);
          } >ram AT>ram :data
          
          .bss : {
                 PROVIDE(bss_start = .);
                 *(.sbss .sbss.*) *(.bss .bss.*);
                 . += 4096;
                 PROVIDE(stack_top = .);
                 . += 4096; 
                 PROVIDE(bss_end = .);
          } >ram AT>ram :bss
    /*             
    PROVIDE(memory_start = ORIGIN(ram));
    PROVIDE(stack_start = bss_end);
    PROVIDE(stack_end = stack_top + 0x8000);
    PROVIDE(memory_end = ORIGIN(ram) + LENGTH(ram));
    PROVIDE(heap_start = stack_end);
    PROVIDE(heap_size = memory_end - heap_start);
    */
    }



