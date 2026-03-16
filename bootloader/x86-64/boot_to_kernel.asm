bits 32
jmp_to_kernel:

mov eax, 0xB8000
mov byte [eax], 'P'
mov byte [eax+1], 0x0F

jmp $


