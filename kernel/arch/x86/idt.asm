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

idt_start:

TIMES 256 dq 0, 0   ; 16 bytes each for 64-bit mode

idt_end:

idt_descriptor:
    dw idt_end - idt_start - 1
    dq idt_start               ; offset
