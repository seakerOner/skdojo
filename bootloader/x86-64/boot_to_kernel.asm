bits 32
jmp_to_kernel:

; we will activate long mode and then jump to kernel
call set_paging

jmp 0x18:(long_mode_entry + 0x10000)           ; 0x18 is 64-bit code

 bits 64

; gdt descriptor 64 bits for high memory
high_gdt_descriptor:
    dw gdt_end - gdt_start - 1 
    dq (gdt_start + 0x10000 + KERNEL_BASE)

kernel_stack_top:
    dq 0x90000 + KERNEL_BASE

KERNEL_BASE equ 0xFFFFFF8000000000

long_mode_entry:

mov ax, 0x10
mov ds, ax 
mov es, ax 
mov ss, ax

mov rbp, 0x90000 + KERNEL_BASE
mov rsp, rbp

mov rax, kernel_stack_top + 0x10000 + KERNEL_BASE
mov [tss + KERNEL_BASE + 0x10000 + 4], rax

; set GDT to high memory
mov rax, tss + 0x10000 + KERNEL_BASE
mov word [gdt_tss + 010000 + 2], ax
shr rax, 16
mov byte [gdt_tss + 0x10000 + 4], al
shr rax, 8
mov byte [gdt_tss + 0x10000 + 7], al
shr rax, 8
mov dword [gdt_tss + 0x10000 + 8], eax

lgdt [high_gdt_descriptor + 0x10000 + KERNEL_BASE]

mov rax, reload + 0x10000
jmp rax

reload:
mov ax, 0x10 
mov ds, ax 
mov es, ax 
mov ss, ax

mov ax, GDT_TSS_SEG 
ltr ax

; address passed as an argument to the kernel with relevant information 
mov rdi, 0x50000 + KERNEL_BASE

mov rax, MEMMAP+0x10000
mov [rdi], rax 

movzx rax , word [MEMMAP_ENTRIES+0x10000]   ; zero extend to 64bits
mov [rdi+8], rax

mov rax, pml4_table+0x10000
mov [rdi+16], rax

mov rax, pdpt_table+0x10000
mov [rdi+24], rax

mov rax, pd_table+0x10000
mov [rdi+32], rax

mov rax, pt_table+0x10000
mov [rdi+40], rax
 
mov rdi, 0x50000 + KERNEL_BASE
mov rax, KERNEL_BASE + 0x20000
jmp rax

; If the kernel ever returns the bootloader just hangs
jmp $

bits 32
set_paging:
    pusha

    mov edi, pml4_table+0x10000
    mov ecx, (4096 * 4)/4  
    xor eax, eax            ; fill with 0's
    rep stosd

    ; PDPT -> PD
    mov eax, pd_table+0x10000
    or eax, 0b11
    mov [pdpt_table+0x10000], eax

    ; PML4 -> PDPT
    mov eax, pdpt_table+0x10000
    or eax, 0b11                ; present + writable
    mov [pml4_table+0x10000], eax

    ; PML4[511] -> PDPT
    mov eax, pdpt_table+0x10000
    or eax, 0b11                ; present + writable
    mov [pml4_table+0x10000 + 511*8], eax

    ; PD -> PT
    mov eax, pt_table+0x10000
    or eax, 0b11 
    mov [pd_table+0x10000], eax

    mov ecx, 512
    mov edi, pt_table+0x10000
    mov eax, 0 

    .fill_pt:
    mov ebx, eax 
    or ebx, 0b11                ; present + writable
    mov [edi], ebx 

    add eax, 0x1000             ; next page (4KB)
    add edi, 8                  ; next entry
    loop .fill_pt

    ; enable PAE
    mov eax, cr4
    or eax, (1 << 5)            ; PAE
    mov cr4, eax

    ; load PML4
    mov eax, pml4_table+0x10000
    mov cr3, eax

    ; activate long mode (MSR)
    mov ecx, 0xC0000080         ; EFER
    rdmsr
    or eax, (1 << 8)            ; LME
    wrmsr

    ; enable paging
    mov eax, cr0 
    or eax, (1 << 31)           ; PG
    mov cr0, eax

    popa
    ret

    align 4096
    pml4_table:
    resb 4096

    align 4096
    pdpt_table:
    resb 4096 

    align 4096
    pd_table:
    resb 4096

    align 4096 
    pt_table:
    resb 4096
