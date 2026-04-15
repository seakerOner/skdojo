; REMAP PIC (Programmable Interrupt Controller) to resolve interrupts conflicts in protected mode
;
; By default IRQs (Interrupt Request or Hardware interrupt) on PIC start at 8 
; But interrupts 0-31 = Exceptions errors from CPU
; IRQs must start at 32+

bits 64 
section .text

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

; (for timer) channel 0, lobyte/hibyte, mode 3 (square wave)
mov al, 00110110b
out 0x43, al 

; PIT base clock / 800 = freq ==> PIT base clock / freq = Hz ==> 1 / Hz = tick_s  ~800Hz (1 tick = ~1.25ms)
mov ax, 1193182 / 800
out 0x40, al    ; low byte 
mov al, ah 
out 0x40, al    ; hight byte

; disable all IRQs
; mov al, 0xFF
; out 0x21, al
; out 0xA1, al

mov al, 11111100b      ; IRQ0 (timer) / IRQ1 (keyboard)
out 0x21, al
mov al, 0xFF
out 0xA1, al

ret

