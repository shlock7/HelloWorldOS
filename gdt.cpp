#include "gdt.h"

GlobalDescriptorTable::GlobalDescriptorTable()
    : nullSegmentDescriptor(0, 0, 0),
    unusedSegmentDescriptor(0, 0, 0),
    codeSegmentDescriptor(0, 64 * 1024 * 1024, 0x9a),   // 64M 内存
    dataSegmentDescriptor(0, 64 * 1024 * 1024, 0x92) {
    uint32_t i[2];             // GDTR,48bit
    i[1] = (uint32_t)this;     // GDT的首地址, 32bit
    i[0] = sizeof(GlobalDescriptorTable) << 16;  // 这个值将被加载到 GDTR 寄存器的高 16 位 16+32=48 bit
    // 通过 lgdt 汇编指令加载 GDTR 寄存器。
    asm volatile("lgdt (%0)"
        :                               /* outputs */
        : "p" (((uint8_t *)i) + 2)      /* inputs */
        );
}

GlobalDescriptorTable::~GlobalDescriptorTable() {}

// 获取数据段的地址
uint16_t GlobalDescriptorTable::DataSegmentSelector() {
    return ((uint8_t*)&dataSegmentDescriptor - (uint8_t*)this);
}

// 代码段的地址
uint16_t GlobalDescriptorTable::CodeSegmentSelector() {
    return ((uint8_t*)&codeSegmentDescriptor - (uint8_t*)this);
}

GlobalDescriptorTable::SegmentDescriptor::SegmentDescriptor(uint32_t base, 
                                                            uint32_t limit, 
                                                            uint8_t type) {
    uint8_t* target = (uint8_t*)this;

    if (limit < 65536) {  // 表示寻址能力限制在2^16次方内, 不需要用保护模式
        target[6] = 0x40; // DB位设置为 1
    }
    else { // 否则的话就需要使用保护模式进行2^20次方寻址
        // G 位和 DB 位都设置为 1 最小寻址单位设置为页(4KB)
        target[6] = 0xC0;

        // 判断limit是否为 4KB的整数倍
        if ((limit & 0xfff) != 0xfff) { // 不是 4KB的整数倍，需要缺失 1 页(4KB)
            limit = (limit >> 12) - 1;
        }
        else { // 如果是全部为 1，则说明已经是 4KB 的整数倍，不需要再减去 1
            limit = limit >> 12;
        }
    }

    // 设置限制字段
    target[0] = limit & 0xff;
    target[1] = (limit >> 8) & 0xff;
    // 或运算是为了设置 target[6] 的低四位而不影响高四位
    target[6] |= (limit >> 16) & 0xf;

    // 设置基地址字段
    target[2] = base & 0xff;
    target[3] = (base >> 8) & 0xff;
    target[4] = (base >> 16) & 0xff;
    target[7] = (base >> 24) & 0xff;

    target[5] = type;
}

// 从段描述符的目标地址中提取出基地址
uint32_t GlobalDescriptorTable::SegmentDescriptor::Base() {
    uint8_t* target = (uint8_t*)this;
    uint32_t result = target[7];
    result = (result << 8) + target[4];
    result = (result << 8) + target[3];
    result = (result << 8) + target[2];
    return result;
}

// 从段描述符的目标地址中提取出段界限
uint32_t GlobalDescriptorTable::SegmentDescriptor::Limit() {
    uint8_t* target = (uint8_t*)this;
    uint32_t result = target[6] & 0xf;
    result = (result << 8) + target[1];
    result = (result << 8) + target[0];
    
    // 检查 段描述符的第 6 个字节的两个高位是否为 1
    // 也就是检查G 位和 DB 位是否都为 1
    // 如果是，说明采用了 4KB 页面大小
    if ((target[6] & 0xC0) == 0xC0)
        result = (result << 12) | 0xfff;
    
    return result;
}