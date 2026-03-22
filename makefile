# $@ = target file
# $< = first dependency
# $^ = all dependencies

ARQ_X86_64 = 0
ARQ_ARM	   = 1

ARQ = $(ARQ_X86_64)

x86CC = nasm
x86VM = qemu-system-x86_64
x86BT_PATH = ./bootloader/x86-64

ARCH_PATH = NULL

CC = gcc
LD = ld

ifeq ($(ARQ), $(ARQ_X86_64))
	ASMCC 	= $(x86CC)
	VM	  	= $(x86VM)
	BT_PATH = $(x86BT_PATH)
	ARCH_PATH = ./kernel/arch/x86
endif

ifeq ($(ARQ), $(ARQ_ARM))
	@echo "This arquitecture is not implemented"
endif

# create image ---

dojo.img: bootloader_stage1.bin bootloader_stage2.bin kernel.bin
	cat ./build/bootloader_stage1.bin \
		./build/bootloader_stage2.bin \
		./build/kernel.bin > ./build/$@

# bootloader ---

bootloader_stage1.bin: $(BT_PATH)/boot_section_stage1.asm
	$(ASMCC) $< -f bin -o ./build/$@

bootloader_stage2.bin: $(BT_PATH)/boot_section_stage2.asm
	$(ASMCC) $< -f bin -o ./build/$@

# kernel ---

kernel.o: ./kernel/kernel.c 
	$(CC) -ffreestanding -nostdlib -m64 -c $< -o ./build/$@

#
# DRIVERS
#
vga.o: ./kernel/drivers/video/vga/vga.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

vgadriver.o: ./kernel/drivers/video/vga/vga_driver.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

ps2keyboard.o: ./kernel/drivers/keyboard/ps2.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

#
# CPU INTERRUPTS
#
k_interrupts.o: ./kernel/interrupts/k_interrupts.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

interrupts.o: $(ARCH_PATH)/interrupts.asm
	$(ASMCC) -f elf64 $< -o ./build/$@

#
# KERNEL EXTENSIONS
#

themes.o: ./kernel/themes/themes.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

printk.o: ./kernel/printk/printk.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

terminal.o: ./kernel/terminal/terminal.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

#
# SENSEIS
#

videosensei.o: ./kernel/video/video_sensei.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

wmanagersensei.o: ./kernel/video/wmanager_sensei.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

keyboardsensei.o: ./kernel/keyboard/keyboard_sensei.c
	$(CC) -ffreestanding -nostdlib -m64 -c $^ -o ./build/$@

#
# Build kernel
#

KERNEL_OBJS = kernel.o interrupts.o k_interrupts.o vga.o vgadriver.o themes.o printk.o terminal.o videosensei.o wmanagersensei.o ps2keyboard.o keyboardsensei.o
KERNEL_OBJS_BUILD = $(addprefix ./build/, $(KERNEL_OBJS))

kernel.bin: $(KERNEL_OBJS)
	$(LD) -nostdlib -m elf_x86_64 -Ttext 0x20000 -e kmain $(KERNEL_OBJS_BUILD) -o ./build/kernel.elf
	objcopy -O binary ./build/kernel.elf ./build/kernel.bin

# commands ---

run: dojo.img
	$(VM) -drive format=raw,if=ide,file=./build/dojo.img -net none 

rundebug: dojo.img
	$(VM) \
		-drive file=./build/dojo.img,format=raw \
		-net none -d int,cpu_reset \
		-debugcon stdio \
		-global isa-debugcon.iobase=0xe9 \
		-no-reboot 

clean:
	rm -f ./build/*

