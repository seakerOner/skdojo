bits 64
default rel

global init_interrupts_x86
extern irq1_kernel_intrpt
extern irq0_kernel_intrpt

section .text

%macro SET_IDT_ENTRY 2
lea rax, [%2]

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
    push rbp 
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    call remap_pic
    ; ----
    ; Double Fault 
    ; SET_IDT_ENTRY idt_start + 8 * 16, isr_df
    ; Page Fault
    ; SET_IDT_ENTRY idt_start + 14 * 16, isr_pf
    ; General protection 
    ; SET_IDT_ENTRY idt_start + 13 * 16, isr_gp 
    ; set IRQ0 (32) TIMER TICK
    SET_IDT_ENTRY idt_start + 32 * 16, irq0_stub
    ; set IRQ1 (33) KEYBOARD DATA READY
    SET_IDT_ENTRY idt_start + 33 * 16, irq1_stub

    ; load IDT (Interrupt Descriptor table)
    lidt [idt_descriptor]

    sti                     ; enable interrupts back on

    pop r15
    pop r14
    pop r13 
    pop r12 
    pop r11
    pop r10 
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax
    ret

; page fault
isr_pf:
    cli
    hlt

; general protection
isr_gp:
    cli
    hlt

; double fault
isr_df:
    cli 
    hlt

; TIMER
irq0_stub:
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp 
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15

call irq0_kernel_intrpt

mov al, 0x20
out 0x20, al

pop r15
pop r14
pop r13 
pop r12 
pop r11
pop r10 
pop r9
pop r8
pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax
iretq

; KEYBOARD DATA READY
irq1_stub:
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp 
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15

call irq1_kernel_intrpt

mov al, 0x20
out 0x20, al

pop r15
pop r14
pop r13 
pop r12 
pop r11
pop r10 
pop r9
pop r8
pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax

iretq



