bits 32
jmp_to_kernel:

; we will activate long mode and then jump to kernel
; TODO: activate long mode

; for now kernel is in 32bits
mov eax, 0x20000            ; kernel address
call eax                    

; If the kernel ever returns the bootloader just hangs
jmp $


