;
; Function to print a null terminated string
;
; ARGS:
;       si -> string addr
;

rm_print_string:                
    pusha
    mov ah, 0x0e                ; teletype output character

rm_next_byte:
    lodsb                       ; AL = [SI], SI++           
    cmp al, 0
    je rm_done
    int 0x10                    ; invoke BIOS video service
    jmp rm_next_byte

rm_done:
    popa
    ret

;
; Function that prints EAX as 8 HEX characters
;
; ARGS:
;       eax -> number
;

rm_print_hex32:
    pusha
    
    mov cx, 8

.hex_loop:
    
    rol eax, 4                  ; move next nibble to low 4 bits

    ; BL register -> low 8 bits of 32bit EBX register
    ; AL register -> low 8 bits of 32bit EAX register
    mov bl, al
    and bl, 0x0F

    cmp bl, 9
    jbe .digit

    add bl, 7                   ; convert A-F

.digit:
    add bl, '0'

    mov ah, 0x0e
    mov al, bl
    int 0x10
    loop .hex_loop

    popa
    ret

MSG_NEWLINE:
    db 10, 13, 0

