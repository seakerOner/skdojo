
void kmain() {
    volatile char* vga = (volatile char*)0xB8000;

    vga[0] = 'K';
    vga[1] = 0x0F;

    while (1);
}
