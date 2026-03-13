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

