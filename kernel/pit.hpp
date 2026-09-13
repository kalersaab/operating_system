#pragma once

#include <stdint.h>

namespace PIT
{
    void initialize(uint32_t frequency);

    uint64_t getTicks();
}