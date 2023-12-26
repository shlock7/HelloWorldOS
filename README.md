# HelloWorldOS
一个手写操作系统的简单实践。

## 在虚拟机(VirtualBox)上运行
- sudo apt install virtualbox
- sudo apt install g++ binutils libc6-dev-i386 xorriso
- make mykernel.iso

第一次运行先在`VirtualBox`里手动新建一个名为`HelloWorldOS`的虚拟机，类型和版本都选Other，内存默认即可，选择不添加虚拟硬盘，然后启动。这时会让选择一个启动盘，选择我们`make mykernel.iso`生成的`mykernel.iso`即可启动。 以后再运行，只需`make run`即可。

## 在物理机上运行
修改原来操作系统的`/boot/grub/grub.cfg`文件。末尾添加如下内容
```
### BEGIN HelloWorldOS ###
menuentry 'HelloWorldOS' {
        multiboot /boot/mykernel.bin
        boot
}
### END HelloWorldOS ###
```

- `make install`
- 重启机器，选择`HelloWorldOS`


在物理机上运行借助了本来的引导程序，添加了一个启动项。