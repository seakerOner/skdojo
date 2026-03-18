;
; This subroutine creates the IDT (Interrupt Descriptor Table)
;
; IDT is a data structure used by the x86 arquiteture
;
; It is the Protected Mode and Long Mode counterpart to the 
; Real Mode Interrupt Vector Table (IVT) telling the CPU where the 
; Interrupt Service Routines (ISR) are located (one per interrupt vector). 
; It is similar to the Global Descriptor Table in structure.
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


idt_start:

TIMES 256 dq 0

idt_end:

idt_descriptor:
    dw idt_end - idt_start - 1
    dd idt_start               ; offset
