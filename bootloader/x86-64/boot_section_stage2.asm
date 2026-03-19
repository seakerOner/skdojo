bits 16
org 0

jmp start

%include "./bootloader/x86-64/rm_printstring.asm"
%include "./bootloader/x86-64/rm_a20bus.asm"
%include "./bootloader/x86-64/rm_gdt.asm"
%include "./bootloader/x86-64/rm_ram_detection.asm"

start_boot_pt2:
db "STARTING Seaker's Dojo BOOT STAGE 2...", 10, 13, 0 

end_boot_pt2:
db "ENDING Seaker's Dojo BOOT STAGE 2...", 10, 13, "ENTERING PROTECTED MODE/ LONG MODE AND GOING TO KERNEL..", 10 , 13, 0

start:

; init registers
cli                             ; clear interrupt flag

; since we updated the code segment we must update DS, ES, SS 
; (data segment/ extra segment/ stack segment respectively)
mov ax, cs
mov ds, ax
mov es, ax

sti                                 

mov si, start_boot_pt2
call rm_print_string

; BOOTLOADER STAGE 2 WILL:
;
; enable A20
; detect RAM
; set GDT (Global Descriptor Table)
; load KERNEL
; enter protected mode
; enter long mode
; jump to kernel



; check if a20 is enabled with BIOS interrupt. if not, enable it
call setbus_a20

; detect RAM on system (what is reserved, bad memory, reclaimable and usable)
call detect_ram

mov si, MSG_NEWLINE
call rm_print_string

mov si, end_boot_pt2
call rm_print_string

; load GDT (Global Descriptor Table) for protected mode
call rm_to_pm                       ; changes to protected mode and goes to 'jump_to_kernel'


%include "./bootloader/x86-64/rm_to_pm.asm"

; 4 sectors
times ((512 * 3) + 510) - ($ - $$) db 0       ;  fill the rest of the boot sector with 0's
dw 0xAA55                       ;  last 2 bytes are the magic number signaling the end of the boot sector
