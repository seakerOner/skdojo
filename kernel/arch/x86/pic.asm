; REMAP PIC (Programmable Interrupt Controller) to resolve interrupts conflicts in protected mode
;
; By default IRQs (Interrupt Request or Hardware interrupt) on PIC start at 8 
; But interrupts 0-31 = Exceptions errors from CPU
; IRQs must start at 32+

bits 64 

; MASTER PIC -> 0x20 (command), 0x21 (data)
; SLAVE  PIC -> 0xA0 (command), 0xA1 (data)

remap_pic:
; set initialization mode
mov al, 0x11
out 0x20, al
out 0xA0, al

; where interrupts start
mov al, 0x20    ; 32
out 0x21, al    ; master
mov al, 0x28    ; 40
out 0xA1, al    ; slave

; tell slave is on master's IRQ2 
mov al, 0x04
out 0x21, al
mov al, 0x02
out 0xA1, al

; 8086 mode (needed for protected mode)
mov al, 0x01
out 0x21, al
out 0xA1, al

; disable all IRQs
; mov al, 0xFF
; out 0x21, al
; out 0xA1, al

mov al, 11111101b      ; only IRQ1 active (keyboard)
out 0x21, al
mov al, 0xFF
out 0xA1, al

ret

