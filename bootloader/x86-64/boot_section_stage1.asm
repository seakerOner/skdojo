
bits 16                         ; init 16 bit mode instructions
org 0x7c00                      ; where BIOS loads the boot sector

jmp start

;
;
; includes

%include "./bootloader/x86-64/rm_printstring.asm"

; globals

start_boot_pt1:
db "STARTING Seaker's Dojo BOOT STAGE 1...", 10, 13, 0 

end_boot_pt1:
db "ENDING Seaker's Dojo BOOT STAGE 1...", 10, 13, 0

bios_ext_unsupported:
mov si, MSG_BIOS_EXTENSIONS_ERR
call rm_print_string
jmp $

MSG_BIOS_EXTENSIONS_ERR:
db "[ERROR] BIOS extensions for disk UNSUPPORTED", 10, 13, 0 

BOOT_DRIVE: db 0
BOOT_STAGE2_OFFSET equ 0x1000
KERNEL_OFFSET equ 0x2000

;
;
; start of the boot_section stage 1

start:

; init registers
cli                             ; clear interrupt flag
xor ax, ax
mov ds, ax
mov es, ax

mov bp, 0x9000                  ; 5 kB stack (0x9000 - 0x7c00)
mov sp, bp
sti                             ; set interrupt flag

mov [BOOT_DRIVE], dl            ; save the disk identifier from where the boot came from

mov si, start_boot_pt1
call rm_print_string

mov ah, 0x41 
mov bx, 0x55AA
int 0x13        ; verify support for extensions

jc bios_ext_unsupported
cmp bx, 0xAA55
jne bios_ext_unsupported

; load disk segments to load stage2 of bootloader into RAM
;
; stage2 will:
;   prepare CPU (protected mode, long mode, etc)
;   load kernel

mov si, dap_stage2
mov ah, 0x42
mov dl, [BOOT_DRIVE]
int 0x13

; This disk load version has a max of 25 sectors
; so we use extended bios disk read

mov si, dap_kernel
mov ah, 0x42
mov dl, [BOOT_DRIVE]
int 0x13

mov si, end_boot_pt1
call rm_print_string

jmp 0x1000:0               ; bootloader stage 2

dap_stage2:
db 0x10 
db 0 
dw 41                         ; num of sectors (max 125, if more is needed more calls are made)
dw 0x0000                     ; offset
dw 0x1000 
dd 1                          
dd 0

dap_kernel:
db 0x10 
db 0 
dw 125                        ; num of sectors (max 125, if more is needed more calls are made)
dw 0x0000                     ; offset
dw 0x2000 
dd 42                         ; sector ( sector 5 = end of stage 2 )
dd 0

times 510 - ($ - $$) db 0       ;  fill the rest of the boot sector with 0's
dw 0xAA55                       ;  last 2 bytes are the magic number signaling the end of the boot sector
