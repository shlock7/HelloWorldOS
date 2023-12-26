# 传递给g++编译器的参数, -m32表示使用32位目标架构。
GPPPARAMS = -m32 -fno-use-cxa-atexit -fleading-underscore -fno-exceptions -fno-builtin -nostdlib -fno-rtti -fno-pie

ASPARAMS = --32		  # 传递给汇编器的参数
LDPARAMS = -melf_i386 -no-pie # 存储了传递给链接器的参数，-melf_i386指定生成ELF格式的32位目标文件


objects = loader.o kernel.o # 构建目标文件的源文件列表

# $@ 目标文件名
# $< 第一个依赖文件
%.o: %.cpp
	g++ ${GPPPARAMS} -o $@ -c $<

# 如何从.s文件生成对应目标文件 .o 
%.o: %.s
	as ${ASPARAMS} -o $@ $<

mykernel.bin: linker.ld ${objects}
	ld ${LDPARAMS} -T $< -o $@ ${objects}


install: mykernel.bin
	sudo cp $< /boot/mykernel.bin

mykernel.iso: mykernel.bin
	mkdir iso
	mkdir iso/boot
	mkdir iso/boot/grub
	cp $< iso/boot/
	echo 'set timeout=0' > iso/boot/grub/grub.cfg
	echo 'set default=0' >> iso/boot/grub/grub.cfg
	echo '' >> iso/boot/grub/grub.cfg
	echo 'menuentry "HelloWorldOS" {' >> iso/boot/grub/grub.cfg
	echo '  multiboot /boot/mykernel.bin' >> iso/boot/grub/grub.cfg
	echo '  boot' >> iso/boot/grub/grub.cfg
	echo '}' >> iso/boot/grub/grub.cfg
	grub-mkrescue --output=$@ iso
	rm -rf iso

clean:
	rm -r *.o
	rm -r *.bin