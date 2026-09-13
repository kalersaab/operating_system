#include "heap.hpp"

namespace
{
    constexpr uint64_t HEAP_SIZE = 1024 * 1024;
    constexpr uint64_t ALIGNMENT = 16;

    struct Block
    {
        uint64_t size;
        bool free;
        Block* next;
    };

    alignas(16) uint8_t heapStorage[HEAP_SIZE];
    Block* firstBlock;
    uint64_t allocatedBytes = 0;

    uint64_t alignSize(uint64_t size)
    {
        return (size + ALIGNMENT - 1) & ~(ALIGNMENT - 1);
    }

    void split(Block* block, uint64_t size)
    {
        if (block->size < size + sizeof(Block) + ALIGNMENT)
            return;

        auto* remainder = reinterpret_cast<Block*>(
            reinterpret_cast<uint8_t*>(block + 1) + size
        );

        remainder->size = block->size - size - sizeof(Block);
        remainder->free = true;
        remainder->next = block->next;

        block->size = size;
        block->next = remainder;
    }
}

namespace Heap
{
    void initialize()
    {
        firstBlock = reinterpret_cast<Block*>(heapStorage);
        firstBlock->size = HEAP_SIZE - sizeof(Block);
        firstBlock->free = true;
        firstBlock->next = nullptr;
        allocatedBytes = 0;
    }

    void* allocate(uint64_t size)
    {
        if (size == 0)
            return nullptr;

        size = alignSize(size);

        for (Block* block = firstBlock; block; block = block->next)
        {
            if (block->free && block->size >= size)
            {
                split(block, size);
                block->free = false;
                allocatedBytes += block->size;
                return block + 1;
            }
        }

        return nullptr;
    }

    void deallocate(void* address)
    {
        if (!address)
            return;

        auto* block = reinterpret_cast<Block*>(address) - 1;

        if (!block->free)
        {
            block->free = true;
            allocatedBytes -= block->size;
        }

        for (Block* current = firstBlock; current && current->next;)
        {
            if (current->free && current->next->free)
            {
                current->size += sizeof(Block) + current->next->size;
                current->next = current->next->next;
            }
            else
            {
                current = current->next;
            }
        }
    }

    uint64_t usedBytes()
    {
        return allocatedBytes;
    }
}

extern "C"
void* kmalloc(uint64_t size)
{
    return Heap::allocate(size);
}

extern "C"
void kfree(void* address)
{
    Heap::deallocate(address);
}
