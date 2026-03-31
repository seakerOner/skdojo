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

; address passed as an argument to the kernel with relevant information 
mov rdi, 0x50000    

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
 
mov rdi, 0x50000
mov rax, 0x20000
call rax

; If the kernel ever returns the bootloader just hangs
jmp $

bits 32
set_paging:
    pusha

    mov edi, pml4_table+0x10000
    mov ecx, (4096 * 4)/4  
    xor eax, eax            ; fill with 0's
    rep stosd

    ; PML4 -> PDPT
    mov eax, pdpt_table+0x10000
    or eax, 0b11                ; present + writable
    mov [pml4_table+0x10000], eax

    ; PDPT -> PD
    mov eax, pd_table+0x10000
    or eax, 0b11
    mov [pdpt_table+0x10000], eax

    ; ; PD -> 2MB identity map    (kernel is responsible to load more memory)
    ; mov eax, 0x00000000         ; physical memory 0
    ; or eax, 0b10000011          ; present + writable + huge page
    ; mov [pd_table], eax

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

    ; VA 0x0 -> PA 0x0 (4KB granularity)

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

