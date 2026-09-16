#pragma once

#include <stdint.h>

namespace Heap
{
    void initialize();
    void* allocate(uint64_t size);
    void* allocateZeroed(uint64_t count, uint64_t size);
    void deallocate(void* address);
    uint64_t usedBytes();
}

extern "C"
{
    void* kmalloc(uint64_t size);
    void* kcalloc(uint64_t count, uint64_t size);
    void kfree(void* address);
}
