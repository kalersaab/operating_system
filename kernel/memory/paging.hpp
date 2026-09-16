#pragma once

#include <stdint.h>

namespace Paging
{
    uintptr_t currentPml4();

    uint16_t pml4Index(uintptr_t address);
    uint16_t pdptIndex(uintptr_t address);
    uint16_t pdIndex(uintptr_t address);
    uint16_t ptIndex(uintptr_t address);
    uint16_t pageOffset(uintptr_t address);
}
