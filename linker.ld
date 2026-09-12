ENTRY(_start)

SECTIONS
{
    . = 1M;

    .text ALIGN(16) :
    {
        KEEP(*(.multiboot))
        *(.text)
        *(.text.*)
    }

    .rodata ALIGN(16) :
    {
        *(.rodata)
        *(.rodata.*)
    }

    .data ALIGN(16) :
    {
        *(.data)
        *(.data.*)
    }

    .bss ALIGN(4096) :
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)

        . = ALIGN(4096);
    }

    kernel_end = .;
}