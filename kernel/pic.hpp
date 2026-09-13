#pragma once

#include <stdint.h>

namespace PIC
{
    void initialize();

    void sendEndOfInterrupt(uint8_t irq);

    void mask(uint8_t irq);

    void unmask(uint8_t irq);
}