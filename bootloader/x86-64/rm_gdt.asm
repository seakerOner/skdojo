;
; This subroutine creates the GDT (Global Descriptor Table)
; GDT is a data structure used by the x86 arquiteture to define and manage memory segments
; It allows us to enable protected mode
;
; NOTE:
;       This is the bare minimum implementation for a valid protected mode
;
; Each entry of the GDT has 8 bytes
;
; REF: https://wiki.osdev.org/Global_Descriptor_Table#Segment_Descriptor
;
;
; TODO:
;       Decide if we want user land with DPL as such with user code/data
;
;       Decide to add TSS (Task state segments)
;       used for:
;           --> Interrupts
;           --> Multitasking
;           --> stack switching
;

gdt_start:
gdt_null: 
    dd 0
    dd 0
    ;dq 0
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
gdt_end:

gdt_descriptor:
dw gdt_end - gdt_start - 1
dd (gdt_start + 0x10000)

GDT_CODE_SEG equ gdt_code - gdt_start
GDT_DATA_SEG equ gdt_data - gdt_start

; code -> base=0 limit=4GB executable readable
; data -> base=0 limit=4GB writable

; Flags descrition:
;
; 11001111b        
;
; 1 granularity (4KB pages)
; 1 32-bit segment
; 0 not long mode
; 0 available
; 1111 limit high

