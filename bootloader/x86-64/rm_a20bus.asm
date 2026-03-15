;
; Function that checks for a20 bus gate support/ check if it's enabled/ enable a20 gate if not already
; 
; NOTES:
;       This subroutine only uses BIOS interrupts in real mode
;       It does not use FAST a20 gate method ( TODO )
;       If the BIOS does not support this functionality it simply will hang
;
; ARGS:
;
;

setbus_a20:
pusha

; check a20 support
mov ax, 0x2403
int 0x15
jc a20notsupported

; check a20 bus state
mov ax, 0x2402                  
int 0x15
jc a20state_fail                

; if a20 not enabled, enable it
cmp al, 0
je a20_enable

mov si, MSG_A20_SUCCESS
call rm_print_string

popa
ret

a20_enable:
mov ax, 0x2401
int 0x15
jc a20enable_fail

mov si, MSG_A20_SUCCESS
call rm_print_string
popa
ret

;
; messages
;

MSG_A20_SUCCESS:
db "---> ENABLED A20 GATE BUS", 10, 13, 0

MSG_A20_NOT_SUPPORTED:
db "[FATAL] A20 GATE IS NOT SUPPORTED ON YOUR SYSTEM", 10, 13, 0

MSG_A20_STATUS_FAIL:
db "[ERROR] Failed to get a20 bus state from BIOS...", 10, 13, 0

MSG_A20_ENABLE_FAIL:
db "[ERROR] Failed to enable a20 bus state from BIOS...", 10, 13, 0

a20notsupported:
mov si, MSG_A20_NOT_SUPPORTED
call rm_print_string
jmp $

a20state_fail:
mov si, MSG_A20_STATUS_FAIL
call rm_print_string
jmp $

a20enable_fail:
mov si, MSG_A20_ENABLE_FAIL
call rm_print_string
jmp $


