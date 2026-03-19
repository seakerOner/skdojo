bits 64

global init_interrupts_x86
extern irq1_kernel_intrpt

%macro SET_IDT_ENTRY 2
mov rax, %2 
mov word [%1], ax                   ; offset low
mov word [%1 + 2], 0x18             ; selector  (GDT_CODE64_SEG = 0x18)
mov byte [%1 + 4], 0    
mov byte [%1 + 5], 10001110b        ; present + ring0 + interrupt gate
shr rax, 16 
mov word [%1 + 6], ax               ; offset high
shr rax, 16
mov dword [%1 + 8], eax
mov dword [%1 + 12], 0
%endmacro

%include "./kernel/arch/x86/idt.asm"
%include "./kernel/arch/x86/pic.asm"

;
; Interrupt subroutines
;

init_interrupts_x86:
    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi



    call remap_pic
    ; ----
    ; set IRQ1 (33) KEYBOARD DATA READY
    SET_IDT_ENTRY idt_start + 33 * 16, irq1_stub

    ; load IDT (Interrupt Descriptor table)
    lidt [idt_descriptor]

    sti                     ; enable interrupts back on

    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

; KEYBOARD DATA READY
irq1_stub:
push rax
push rbx
push rcx
push rdx
push rsi
push rdi

call irq1_kernel_intrpt

mov al, 0x20
out 0x20, al

pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax

iretq



