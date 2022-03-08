/*
Alternative linker script
*/

OUTPUT_ARCH("riscv")
ENTRY(_start)


SECTIONS
    {
        . = 0x8000000;
        
        .text : {
            *(.text .text.*)
            . = ALIGN(0x1000);
            _uservec = .;
            *(usersec);
            . = ALIGN(0x1000);
            ASSERT(. - _uservec = 0x1000, "error");
            PROVIDE(etext = .);
        }

        .rodata : {
            . = ALIGN(16);
            *(.srodata .srodata.*)
            . = ALIGN(16);
            *(.rodata .rodata.*)
        }        

        .data {
            . = ALIGN(16);
            *(.sdata .sdata.*)
            . = ALIGN(16);
            *(.data .data.*)
        }

        .bss : {
            . = ALIGN(16);
            *(.sbss .sbss.*)
            . = ALIGN(16)
            *(.bss .bss*)
        }

        PROVIDE(end = .);
    }
