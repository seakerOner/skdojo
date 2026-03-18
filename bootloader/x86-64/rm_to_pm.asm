
%include "./bootloader/x86-64/boot_to_kernel.asm"

bits 16
rm_to_pm:

cli         ; disable interrupts until GDT, IDT, PIC and PROTECTED MODE is configured 

; load GDT (Global Descriptor Table) for protected mode
lgdt [gdt_descriptor]

; ACTIVATE Protected mode
mov eax, cr0
or eax, 1
mov cr0, eax

; do a far jump to flush registers after load GDT.

; we do an offset of 0x10000 because stage2 bootloader on physical memory starts at 0x10000
; but nasm will compile with org 0, so we take that into account
jmp dword GDT_CODE_SEG:(protected_mode_start + 0x10000)

bits 32
protected_mode_start:

mov ax, GDT_DATA_SEG
mov ds, ax
mov es, ax
mov fs, ax
mov gs, ax
mov ss, ax

mov ebp, 0x90000
mov esp, ebp

call jmp_to_kernel

