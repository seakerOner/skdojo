;
; Function to load N sectors from disk where bootloader came from into RAM 
;
; ARGS:
;      al      -> Numbers of sectors to read (must be non-zero)
;      cl      -> sector base
;      ES : BX -> destination buffer
;

disk_load:
    pusha

    mov ah, 2                   ; read disk sector(s) into memory
    mov ch, 0                   ; low eight bits of cylinder number
    mov dh, 0                   ; head number
    mov dl, [BOOT_DRIVE]        ; drive number

    int 0x13

    jc disk_error               ; carry flag = error

    popa
    ret

disk_error:
    mov si, MSG_DISK_ERROR
    call rm_print_string
    jmp $

MSG_DISK_ERROR:
    db "[ERROR] FAILED TO LOAD DISK SECTOR", 10, 13, 0 
