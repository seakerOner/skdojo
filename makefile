# $@ = target file
# $< = first dependency
# $^ = all dependencies

ARQ_X86_64 = 0
ARQ_ARM	   = 1

ARQ = $(ARQ_X86_64)

x86CC 			= nasm
x86VM 			= qemu-system-x86_64
x86BT_PATH 		= ./bootloader/x86-64
x86_ENABLE_SSE 	= 1

ARCH_PATH = NULL

CC = gcc 

CC_FLAGS = -Wall -Wextra   \
	-O2					 \
	-m64 			     \
	-ffreestanding 	     \
	-fno-stack-protector \
	-fno-common			 \
	-fno-merge-constants \
	-fno-asynchronous-unwind-tables \
	-fcf-protection=none \
	-fPIE				 \
	-nostdlib		     \
	-mno-red-zone		 \

ifeq ($(x86_ENABLE_SSE), 0)
	CC_EXT_FLAGS = 	-mno-sse			 \
					-mno-sse2			 \
					-mno-mmx			 
else
	CC_EXT_FLAGS = 	-DX86_ENABLED_SSE 
endif


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
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS)  -c $< -o ./build/$@

#
# DRIVERS
#
vga.o: ./kernel/drivers/video/vga/vga.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

vgadriver.o: ./kernel/drivers/video/vga/vga_driver.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

ps2keyboard.o: ./kernel/drivers/keyboard/ps2.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

#
# CPU INTERRUPTS
#
k_interrupts.o: ./kernel/interrupts/k_interrupts.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

interrupts.o: $(ARCH_PATH)/interrupts.asm
	$(ASMCC) -f elf64 $< -o ./build/$@

#
# PLATFORM DEPENDENT KERNEL EXTENSIONS
#

ifeq ($(x86_ENABLE_SSE), 1)
	x86_sse.o: $(ARCH_PATH)/init_sse.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@
endif

#
# KERNEL EXTENSIONS
#
tatami.o: ./kernel/tatami/tatami.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

process.o: ./kernel/process/process.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

themes.o: ./kernel/themes/themes.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

printk.o: ./kernel/printk/printk.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

terminal.o: ./kernel/terminal/terminal.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

kheap.o: ./kernel/memory/kheap.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

kslab.o: ./kernel/memory/kslab.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

kata.o: ./kernel/memory/kata.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

eventrouter.o: ./kernel/event_router/event_router.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

#
# SENSEIS
#

videosensei.o: ./kernel/video/video_sensei.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

wmanagersensei.o: ./kernel/video/wmanager_sensei.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

compositorsensei.o: ./kernel/video/compositor_sensei.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS)-c $< -o ./build/$@

keyboardsensei.o: ./kernel/keyboard/keyboard_sensei.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

memorysensei.o: ./kernel/memory/memory_sensei.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

processes_sensei.o : ./kernel/process/processes_sensei.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

time_sensei.o: ./kernel/time/time_sensei.c
	$(CC) $(CC_FLAGS) $(CC_EXT_FLAGS) -c $< -o ./build/$@

#
# Build kernel
#

KERNEL_OBJS = kernel.o interrupts.o k_interrupts.o vga.o vgadriver.o themes.o printk.o terminal.o videosensei.o wmanagersensei.o ps2keyboard.o keyboardsensei.o memorysensei.o compositorsensei.o kheap.o tatami.o process.o processes_sensei.o time_sensei.o kslab.o kata.o eventrouter.o

ifeq ($(x86_ENABLE_SSE), 1)
	KERNEL_OBJS += x86_sse.o
endif

KERNEL_OBJS_BUILD = $(addprefix ./build/, $(KERNEL_OBJS))

kernel.bin: $(KERNEL_OBJS)
	$(LD) -nostdlib --no-relax -m elf_x86_64 -T linker.ld $(KERNEL_OBJS_BUILD) -o ./build/kernel.elf
	objcopy -O binary ./build/kernel.elf ./build/kernel.bin

# commands ---

run: dojo.img
	$(VM) -drive format=raw,if=ide,file=./build/dojo.img -net none

rundebug: dojo.img
	$(VM) \
		-drive file=./build/dojo.img,format=raw \
		-net none -d int,cpu_reset \
		-debugcon stdio \
		-m 2G \
		-global isa-debugcon.iobase=0xe9 \
		-no-reboot 

clean:
	rm -f ./build/*

