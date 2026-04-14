;
; This subroutine creates the GDT (Global Descriptor Table)
; GDT is a data structure used by the x86 arquiteture to define and manage memory segments
; It allows us to enable protected mode
;
; When the bootloader enters in 64bit mode, we transition the gdt table to high memory!
;
; NOTE:
;       This is the bare minimum implementation for a valid protected mode
;
; Each entry of the GDT has 8 bytes
;
; REF: https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
;
align 16
tss:
    resb 104

gdt_start:
gdt_null: 
    dd 0
    dd 0
gdt_code: 
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10011010b
    db 11001111b
    db 0x0              
gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0              
gdt_code64:
    dq 0x00AF9A000000FFFF
gdt_tss:
    dw 104 - 1 
    dw 0
    db 0
    db 10001001b            ; TSS available
    db 0
    db 0
    dd 0
    dd 0
gdt_end:

gdt_descriptor:
dw gdt_end - gdt_start - 1
dd (gdt_start + 0x10000)

GDT_CODE_SEG equ gdt_code - gdt_start     ; 0x08
GDT_DATA_SEG equ gdt_data - gdt_start     ; 0x10
GDT_CODE64_SEG equ gdt_code64 - gdt_start ; 0x18
GDT_TSS_SEG equ gdt_tss - gdt_start       ; 0x20
