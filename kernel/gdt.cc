#include "gdt.hh"

#include <stdint.h>

namespace
{
    struct [[gnu::packed]] GDTEntry
    {
        uint16_t limitLow;
        uint16_t baseLow;

        uint8_t baseMiddle;

        uint8_t access;

        uint8_t granularity;

        uint8_t baseHigh;
    };

    struct [[gnu::packed]] GDTPointer
    {
        uint16_t limit;
        uint64_t base;
    };

    GDTEntry gdt[3];
    GDTPointer gdtPointer;

    void setEntry(
        int index,
        uint32_t base,
        uint32_t limit,
        uint8_t access,
        uint8_t granularity)
    {
        gdt[index].limitLow =
            limit & 0xFFFF;

        gdt[index].baseLow =
            base & 0xFFFF;

        gdt[index].baseMiddle =
            (base >> 16) & 0xFF;

        gdt[index].access =
            access;

        gdt[index].granularity =
            ((limit >> 16) & 0x0F)
            | (granularity & 0xF0);

        gdt[index].baseHigh =
            (base >> 24) & 0xFF;
    }
}

namespace GDT
{
    void initialize()
    {
        gdtPointer.limit =
            sizeof(gdt) - 1;

        gdtPointer.base =
            reinterpret_cast<uint64_t>(&gdt);

        // Null descriptor
        setEntry(
            0,
            0,
            0,
            0,
            0
        );

        // Kernel 64-bit code segment
        //
        // Access:
        // Present = 1
        // Ring 0
        // Code/Data = 1
        // Executable = 1
        //
        setEntry(
            1,
            0,
            0xFFFFF,
            0x9A,
            0x20
        );

        // Kernel data segment
        setEntry(
            2,
            0,
            0xFFFFF,
            0x92,
            0xCF
        );

        asm volatile(
            "lgdt %0"
            :
            : "m"(gdtPointer)
            : "memory"
        );

        // Reload data segment registers.
        asm volatile(
            "mov $0x10, %%ax\n"
            "mov %%ax, %%ds\n"
            "mov %%ax, %%es\n"
            "mov %%ax, %%ss\n"
            :
            :
            : "rax", "memory"
        );
    }
}