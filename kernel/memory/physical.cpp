#include "physical.hpp"

namespace
{
    constexpr uintptr_t FRAME_SIZE = 4096;
    constexpr uint32_t FRAME_COUNT = 32768;
    constexpr uint32_t WORD_BITS = 32;

    struct [[gnu::packed]] MultibootTag
    {
        uint32_t type;
        uint32_t size;
    };

    struct [[gnu::packed]] MemoryMapTag
    {
        uint32_t type;
        uint32_t size;
        uint32_t entrySize;
        uint32_t version;
    };

    struct [[gnu::packed]] MemoryMapEntry
    {
        uint64_t address;
        uint64_t length;
        uint32_t type;
        uint32_t reserved;
    };

    uint32_t frameBitmap[FRAME_COUNT / WORD_BITS];
    uint32_t used = 0;
    uint32_t total = 0;

    void markUsed(uint32_t frame)
    {
        uint32_t word = frame / WORD_BITS;
        uint32_t bit = frame % WORD_BITS;

        if ((frameBitmap[word] & (1u << bit)) == 0)
        {
            frameBitmap[word] |= 1u << bit;
            ++used;
        }
    }

    void markFree(uint32_t frame)
    {
        uint32_t word = frame / WORD_BITS;
        uint32_t bit = frame % WORD_BITS;

        if ((frameBitmap[word] & (1u << bit)) != 0)
        {
            frameBitmap[word] &= ~(1u << bit);
            --used;
        }
    }
}

namespace PhysicalMemory
{
    void initialize(uintptr_t multibootInfo, uintptr_t kernelEnd)
    {
        for (uint32_t& word : frameBitmap)
            word = 0xFFFFFFFFu;

        used = FRAME_COUNT;
        total = 0;

        auto* info = reinterpret_cast<uint32_t*>(multibootInfo);
        auto* tag = reinterpret_cast<MultibootTag*>(info + 2);

        while (tag->type != 0)
        {
            if (tag->type == 6)
            {
                auto* map = reinterpret_cast<MemoryMapTag*>(tag);
                auto* entry = reinterpret_cast<MemoryMapEntry*>(
                    reinterpret_cast<uint8_t*>(map) + 16
                );
                auto* end = reinterpret_cast<uint8_t*>(map) + map->size;

                while (reinterpret_cast<uint8_t*>(entry) < end)
                {
                    if (entry->type == 1)
                    {
                        uint64_t start =
                            (entry->address + FRAME_SIZE - 1) & ~(FRAME_SIZE - 1);
                        uint64_t finish =
                            (entry->address + entry->length) & ~(FRAME_SIZE - 1);

                        if (start < FRAME_COUNT * FRAME_SIZE)
                        {
                            if (finish > FRAME_COUNT * FRAME_SIZE)
                                finish = FRAME_COUNT * FRAME_SIZE;

                            for (uint64_t address = start; address < finish;
                                 address += FRAME_SIZE)
                            {
                                uint32_t frame =
                                    static_cast<uint32_t>(address / FRAME_SIZE);
                                markFree(frame);
                                ++total;
                            }
                        }
                    }

                    entry = reinterpret_cast<MemoryMapEntry*>(
                        reinterpret_cast<uint8_t*>(entry) + map->entrySize
                    );
                }
            }

            tag = reinterpret_cast<MultibootTag*>(
                (reinterpret_cast<uintptr_t>(tag) + tag->size + 7) & ~uintptr_t(7)
            );
        }

        used = total;

        uint32_t firstReserved =
            static_cast<uint32_t>((kernelEnd + FRAME_SIZE - 1) / FRAME_SIZE);

        for (uint32_t frame = 0; frame < firstReserved; ++frame)
            markUsed(frame);
    }

    uintptr_t allocateFrame()
    {
        for (uint32_t frame = 0; frame < FRAME_COUNT; ++frame)
        {
            uint32_t word = frame / WORD_BITS;
            uint32_t bit = frame % WORD_BITS;

            if ((frameBitmap[word] & (1u << bit)) == 0)
            {
                markUsed(frame);
                return static_cast<uintptr_t>(frame) * FRAME_SIZE;
            }
        }

        return 0;
    }

    void freeFrame(uintptr_t address)
    {
        if (address == 0 || address % FRAME_SIZE != 0)
            return;

        uint32_t frame = static_cast<uint32_t>(address / FRAME_SIZE);

        if (frame < FRAME_COUNT)
            markFree(frame);
    }

    uint32_t usedFrames()
    {
        return used;
    }

    uint32_t totalFrames()
    {
        return total;
    }
}
