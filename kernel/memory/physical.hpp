#pragma once

#include <stdint.h>

namespace PhysicalMemory
{
    void initialize(uintptr_t multibootInfo, uintptr_t kernelEnd);
    uintptr_t allocateFrame();
    void freeFrame(uintptr_t address);
    uint32_t usedFrames();
    uint32_t totalFrames();
}
