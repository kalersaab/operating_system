#include "pit.hpp"
#include "pic.hpp"
#include "console.hpp"

namespace
{
    constexpr uint16_t PIT_COMMAND = 0x43;
    constexpr uint16_t PIT_CHANNEL0 = 0x40;

    constexpr uint32_t PIT_BASE_FREQUENCY = 1193182;

    volatile uint64_t ticks = 0;

    inline void outb(uint16_t port, uint8_t value)
    {
        asm volatile(
            "outb %0, %1"
            :
            : "a"(value), "Nd"(port)
        );
    }
}

extern "C"
void timer_handler()
{
    ticks = ticks + 1;

    PIC::sendEndOfInterrupt(0);
}

namespace PIT
{
    void initialize(uint32_t frequency)
    {
        if (frequency == 0)
            frequency = 100;

        uint32_t divisor =
            PIT_BASE_FREQUENCY / frequency;

        if (divisor > 65535)
            divisor = 65535;

        if (divisor < 1)
            divisor = 1;

        /*
         * Channel 0
         * Access mode: low byte + high byte
         * Mode 3: square wave
         * Binary mode
         */
        outb(
            PIT_COMMAND,
            0x36
        );

        /*
         * Send divisor low byte.
         */
        outb(
            PIT_CHANNEL0,
            divisor & 0xFF
        );

        /*
         * Send divisor high byte.
         */
        outb(
            PIT_CHANNEL0,
            (divisor >> 8) & 0xFF
        );

        /*
         * Enable IRQ0.
         */
        PIC::unmask(0);
    }

    uint64_t getTicks()
    {
        return ticks;
    }
}