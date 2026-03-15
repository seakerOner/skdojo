;
;   A Function to get usable RAM segments from BIOS
;
;   each entry structure from SMAP 
;
;   base address (64-bit)
;   length       (64-bit)
;   type         (32-bit)
;   acpi ext     (32-bit)
;
;   types:
;   
;   1 = usable RAM
;   2 = reserved
;   3 = ACPI reclaim
;   4 = ACPI NVS
;   5 = bad memory
;

detect_ram:
    pusha

    ; For newer BIOSes - Get system memory map

    mov si, 0
    xor ebx, ebx                    ; start beginning of map
    mov di, MEMMAP

.e820_loop:
    mov eax, 0x0000E820             ; some BIOSes expect the high word of EAX to be clear on entry 

    mov edx, 0x534D4150             ; 'SMAP'
    mov ecx, 24                     ; size of buffer

    int 0x15
    jc .done

    cmp eax, 0x534D4150             
    jne .done

    add di, 24
    inc si

    cmp ebx, 0
    jne .e820_loop

.done:
    mov [MEMMAP_ENTRIES], si

    mov si, MSG_MEMMAP_ENTRIES
    call rm_print_string

    call rm_print_memmap

    popa
    ret


MEMMAP: TIMES 1024 db 0             ; 1024 / 24 = 42 entries

MEMMAP_ENTRIES: dw 0

MSG_MEMMAP_ENTRIES:
    db "---> DETECTED RAM MAPPING ON SYSTEM", 10, 13, 0

MSG_START_E820:
    db 10, 13, "E820 Memory map:", 10, 13, 10, 13, 0

MSG_SEPARATOR:
    db " - ", 0

MSG_USABLE_RAM:
    db " usable RAM", 10, 13, 0

MSG_RESERVED:
    db " reserved", 10, 13, 0

MSG_ACPI_RECLAIM:
    db " ACPI reclaim", 10, 13, 0

MSG_ACPI_NVS:
    db " ACPI NVS", 10, 13, 0

MSG_BAD_MEMORY:
    db " bad memory", 10, 13, 0

rm_print_memmap:
    pusha

    mov si, MSG_START_E820
    call rm_print_string

    mov cx, [MEMMAP_ENTRIES]
    mov di, MEMMAP
    
.next_mmap_section:
    cmp cx, 0
    je .done_print_mmap

    ; base address
    mov eax, [di+4]
    call rm_print_hex32

    mov eax, [di]
    call rm_print_hex32

    mov si, MSG_SEPARATOR
    call rm_print_string

    ; length 
    mov eax, [di+12]
    call rm_print_hex32

    mov eax, [di+8]
    call rm_print_hex32

    ; type
    mov eax, [di+16]

    cmp al, 1
    je .print_usable_ram
    cmp al, 2
    je .print_reserved
    cmp al, 3 
    je .print_acpi_reclaim
    cmp al, 4 
    je .print_acpi_nvs
    cmp al, 5 
    je .print_bad_memory
    jmp .after_type

.print_usable_ram:
    mov si, MSG_USABLE_RAM
    call rm_print_string
    jmp .after_type
.print_reserved:
    mov si, MSG_RESERVED
    call rm_print_string
    jmp .after_type
.print_acpi_reclaim:
    mov si, MSG_ACPI_RECLAIM
    call rm_print_string
    jmp .after_type
.print_acpi_nvs:
    mov si, MSG_ACPI_NVS
    call rm_print_string
    jmp .after_type
.print_bad_memory:
    mov si, MSG_BAD_MEMORY
    call rm_print_string
    jmp .after_type

.after_type:
    add di, 24
    dec cx
    jmp .next_mmap_section

.done_print_mmap:
    popa
    ret

