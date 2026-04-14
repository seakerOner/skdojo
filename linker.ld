ENTRY(kmain)

SECTIONS
{
    . = 0xFFFFFF8000020000; 

    .text : ALIGN(4K)
    {
        *(.text*)
    }

    .rodata : ALIGN(4K) 
    {
        *(.rodata*)
    }

    .data : ALIGN(4K) 
    {
        *(.data*)
    } 

    .bss : ALIGN(4K) 
    {
        *(COMMON)
        *(.bss*)
    } 

    .bootstrap 0x100000 : AT(0x100000) ALIGN(4K) 
    {
        __bootstrap_start = .;
        *(.bootstrap*)
        __bootstrap_end = .;
    }
}
