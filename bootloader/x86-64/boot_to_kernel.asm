bits 32
jmp_to_kernel:

; we will activate long mode and then jump to kernel
; TODO: activate long mode

; for now kernel is in 32bits
; jmp 0x08:0x20000            ; GDT_CODE_SEG = 0x08
mov eax, 0x20000
call eax

; If the kernel ever returns the bootloader just hangs
jmp $


