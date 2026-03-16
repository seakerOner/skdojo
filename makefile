# $@ = target file
# $< = first dependency
# $^ = all dependencies

ARQ_X86_64 = 0
ARQ_ARM	   = 1

ARQ = $(ARQ_X86_64)

x86CC = nasm
x86VM = qemu-system-x86_64
x86BT_PATH = ./bootloader/x86-64

ifeq ($(ARQ), $(ARQ_X86_64))
	ASMCC 	= $(x86CC)
	VM	  	= $(x86VM)
	BT_PATH = $(x86BT_PATH)
endif

ifeq ($(ARQ), $(ARQ_ARM))
	@echo "This arquitecture is not implemented"
endif

dojo.img: bootloader_stage1.bin bootloader_stage2.bin
	cat ./build/bootloader_stage1.bin ./build/bootloader_stage2.bin > ./build/$@

bootloader_stage1.bin: $(BT_PATH)/boot_section_stage1.asm
	$(ASMCC) $< -f bin -o ./build/$@

bootloader_stage2.bin: $(BT_PATH)/boot_section_stage2.asm
	$(ASMCC) $< -f bin -o ./build/$@

run: dojo.img
	$(VM) -drive format=raw,file=./build/dojo.img -net none -d int,cpu_reset -no-reboot
	#$(VM) -drive format=raw,file=./build/dojo.img -net none 

clean:
	rm -f ./build/*

