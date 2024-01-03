#include "types.h"
#include "gdt.h"

// 因为我们的操作系统没有TTY IO，所以我们需要重新写一个printf函数
void printf(char* str) {
    // 0xb8000内存地址是显示器地址，往这里写数据就直接能够输出到屏幕上
    static uint16_t* VideoMemory = (uint16_t*)0xb8000;
    
    static uint8_t x = 0, y = 0;    // 光标的坐标位置
    for (int i = 0; str[i]; i++) {
        //写入字符串
        switch(str[i]) {
        case '\n':     // 如果需要换行就跳到下一行
            y++;
            x = 0;
            break;
        default:
        // 取或0xff00的意思是我们需要把屏幕高四位拉低，否则就是黑色的字体，黑色的字体黑色的屏幕是啥也看不到的
            VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | str[i];
            x++;
            break;
        }

        if (x >= 80) { // 这一行写满了 跳到下一行
            x = 0;
            y++;
        }

        if (y >= 25) { // 整个屏幕写满了  清屏
            for (y = 0; y < 25; y++) {
                for (x = 0; x < 80; x++) {
                    // 清屏就是写空字符
                    VideoMemory[80 * y + x] = (VideoMemory[80 * y + x] & 0xFF00) | ' ';
                }
            }
            x = 0, y = 0;
        }

    }
}

// 操作系统构造函数委托方法
typedef void(*constructor)();
// 全局定义构造委托
constructor start_ctors;
// 全局定义析构委托
constructor end_ctors;

// 轮询函数，并且执行
extern "C" void system_constructors(){
	for(constructor* i=&start_ctors;i!=&end_ctors;i+=1){
		(*i)();
	}
}

// 操作系统主启动函数，这里我们打印一个字符串然后让操作系统进入等待
extern "C" void kernelMain(void* multiboot_structure, uint32_t magicnumber) {
    printf((char*)"Hello world!\n");
    printf((char*)"Test newline");
    printf((char*)". Everything is ok.");
    GlobalDescriptorTable gdt;
    
    while(1);
}