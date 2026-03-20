bits 32
jmp_to_kernel:

; we will activate long mode and then jump to kernel
call set_paging

jmp 0x18:(long_mode_entry + 0x10000)           ; 0x18 is 64-bit code

 bits 64

long_mode_entry:

mov ax, 0x10
mov ds, ax 
mov es, ax 
mov ss, ax

mov rbp, 0x90000
mov rsp, rbp


mov rax, 0x20000
call rax

; If the kernel ever returns the bootloader just hangs
jmp $

bits 32
set_paging:
    pusha

    mov edi, pml4_table
    mov ecx, (4096 * 3)/4  
    xor eax, eax            ; fill with 0's
    rep stosd

    ; PML4 -> PDPT
    mov eax, pdpt_table 
    or eax, 0b11                ; present + writable
    mov [pml4_table], eax

    ; PDPT -> PD
    mov eax, pd_table
    or eax, 0b11
    mov [pdpt_table], eax

    ; PD -> 2MB identity map    (kernel is responsible to load more memory)
    mov eax, 0x00000000         ; physical memory 0
    or eax, 0b10000011          ; present + writable + huge page
    mov [pd_table], eax

    ; enable PAE
    mov eax, cr4
    or eax, (1 << 5)            ; PAE
    mov cr4, eax

    ; load PML4
    mov eax, pml4_table
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

