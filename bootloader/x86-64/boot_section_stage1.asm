
bits 16                         ; init 16 bit mode instructions
org 0x7c00                      ; where BIOS loads the boot sector

jmp start

;
;
; includes

%include "./bootloader/x86-64/rm_printstring.asm"
%include "./bootloader/x86-64/rm_disk_load.asm"

; globals

start_boot_pt1:
db "STARTING Seaker's Dojo BOOT STAGE 1...", 10, 13, 0 

end_boot_pt1:
db "ENDING Seaker's Dojo BOOT STAGE 1...", 10, 13, 0

BOOT_DRIVE: db 0
BOOT_STAGE2_OFFSET equ 0x1000

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

; load disk segments to load stage2 of bootloader into RAM
;
; stage2 will:
;   prepare CPU (protected mode, long mode, etc)
;   load kernel

mov ax, BOOT_STAGE2_OFFSET
mov es, ax
mov bx, 0x0000

mov cl, 2                       ; sector ( sector 1 = boot stage 1)
mov al, 3                       ; num of sectors

call disk_load

mov si, end_boot_pt1
call rm_print_string

jmp 0x1000:0               ; bootloader stage 2

times 510 - ($ - $$) db 0       ;  fill the rest of the boot sector with 0's
dw 0xAA55                       ;  last 2 bytes are the magic number signaling the end of the boot sector
