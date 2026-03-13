org 0x10000

jmp start

%include "./bootloader/x86-64/rm_printstring.asm"
; %include "./bootloader/x86-64/rm_disk_load.asm"

start_boot_pt2:
db "STARTING Seaker's Dojo BOOT STAGE 2...", 10, 13, 0 

end_boot_pt2:
db "ENDING Seaker's Dojo BOOT STAGE 2...", 10, 13, 0

start:

mov si, start_boot_pt2
call rm_print_string

mov si, end_boot_pt2
call rm_print_string

; 3 sections
times ((512 * 2) + 510) - ($ - $$) db 0       ;  fill the rest of the boot sector with 0's
dw 0xAA55                       ;  last 2 bytes are the magic number signaling the end of the boot sector
