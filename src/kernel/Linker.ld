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

        




