bits 32

global init_interrupts_x86
extern irq1_kernel_intrpt

%macro SET_IDT_ENTRY 2
mov eax, %2 
mov word [%1], ax                   ; offset low
mov word [%1 + 2], 0x08             ; selector  (GDT_CODE_SEG = 0x08)
mov byte [%1 + 4], 0    
mov byte [%1 + 5], 10001110b        ; present + ring0 + interrupt gate
shr eax, 16 
mov word [%1 + 6], ax               ; offset high
%endmacro

%include "./kernel/arch/x86/idt.asm"
%include "./kernel/arch/x86/pic.asm"

;
; Interrupt subroutines
;

init_interrupts_x86:
    pusha 

    call remap_pic
    ; ----
    ; set IRQ1 (33) KEYBOARD DATA READY
    SET_IDT_ENTRY idt_start + 33 * 8, irq1_stub

    ; load IDT (Interrupt Descriptor table)
    lidt [idt_descriptor]

    sti                     ; enable interrupts back on

    popa 
    ret

; KEYBOARD DATA READY
irq1_stub:
pusha

call irq1_kernel_intrpt

mov al, 0x20
out 0x20, al
popa
iret



