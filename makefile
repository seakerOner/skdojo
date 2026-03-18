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
	$(CC) -ffreestanding -nostdlib -m32 -c $< -o ./build/$@

vga.o: ./kernel/vga/vga.c
	$(CC) -ffreestanding -nostdlib -m32 -c $^ -o ./build/$@

k_interrupts.o: ./kernel/interrupts/k_interrupts.c
	$(CC) -ffreestanding -nostdlib -m32 -c $^ -o ./build/$@

interrupts.o: $(ARCH_PATH)/interrupts.asm
	$(ASMCC) -f elf32 $< -o ./build/$@

KERNEL_OBJS = kernel.o interrupts.o k_interrupts.o vga.o
KERNEL_OBJS_BUILD = $(addprefix ./build/, $(KERNEL_OBJS))

kernel.bin: $(KERNEL_OBJS)
	$(LD) -nostdlib -m elf_i386 -Ttext 0x20000 -e kmain $(KERNEL_OBJS_BUILD) -o ./build/kernel.elf
	objcopy -O binary ./build/kernel.elf ./build/kernel.bin

# commands ---

run: dojo.img
	$(VM) -drive format=raw,file=./build/dojo.img -net none -d int,cpu_reset -no-reboot
	#$(VM) -drive format=raw,file=./build/dojo.img -net none 

clean:
	rm -f ./build/*

