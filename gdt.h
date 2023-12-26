#ifndef __GDT_H
#define __GDT_H

#include "types.h"

class GlobalDescriptorTable {
public:
    class SegmentDescriptor {
    public:
        SegmentDescriptor(uint32_t base, uint32_t limit, uint8_t type);
        uint32_t Base();        // 返回段基址
        uint32_t Limit();       // 返回段界限
    private:
        uint16_t limit_lo;      // 低地址的段界限 0~15
        uint16_t base_lo;       // 低地址的段基址 16~31
        uint8_t base_hi;        // 高地址的段基址 32~39
        uint8_t type;           // Access Byte部分
        uint8_t flags_limit_hi; // Limit和Flags
        uint8_t base_vhi;       // 高地址的段基址 56~63
    } __attribute__((packed));  // 不进行内存对齐

    SegmentDescriptor nullSegmentDescriptor;    // 空的
    SegmentDescriptor unusedSegmentDescriptor;  // 未被使用的
    SegmentDescriptor codeSegmentDescriptor;    // 代码段
    SegmentDescriptor dataSegmentDescriptor;    // 数据段

public:
    GlobalDescriptorTable();
    ~GlobalDescriptorTable();

    uint16_t CodeSegmentSelector();
    uint16_t DataSegmentSelector();
};


#endif