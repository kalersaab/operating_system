#include "paging.hpp"

namespace Paging
{
    constexpr uintptr_t INDEX_MASK = 0x1FF;
    constexpr uintptr_t OFFSET_MASK = 0xFFF;

    uintptr_t currentPml4()
    {
        uintptr_t cr3;

        asm volatile(
            "mov %%cr3, %0"
            : "=r"(cr3)
        );

        return cr3 & ~static_cast<uintptr_t>(0xFFF);
    }

    uint16_t pml4Index(uintptr_t address)
    {
        return static_cast<uint16_t>((address >> 39) & INDEX_MASK);
    }

    uint16_t pdptIndex(uintptr_t address)
    {
        return static_cast<uint16_t>((address >> 30) & INDEX_MASK);
    }

    uint16_t pdIndex(uintptr_t address)
    {
        return static_cast<uint16_t>((address >> 21) & INDEX_MASK);
    }

    uint16_t ptIndex(uintptr_t address)
    {
        return static_cast<uint16_t>((address >> 12) & INDEX_MASK);
    }

    uint16_t pageOffset(uintptr_t address)
    {
        return static_cast<uint16_t>(address & OFFSET_MASK);
    }
}
