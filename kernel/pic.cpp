#include "pic.hpp"

namespace
{
    constexpr uint16_t PIC1_COMMAND = 0x20;
    constexpr uint16_t PIC1_DATA    = 0x21;

    constexpr uint16_t PIC2_COMMAND = 0xA0;
    constexpr uint16_t PIC2_DATA    = 0xA1;

    constexpr uint8_t ICW1_INIT = 0x10;
    constexpr uint8_t ICW1_ICW4 = 0x01;

    constexpr uint8_t ICW4_8086 = 0x01;

    inline void outb(uint16_t port, uint8_t value)
    {
        asm volatile(
            "outb %0, %1"
            :
            : "a"(value), "Nd"(port)
        );
    }

    inline uint8_t inb(uint16_t port)
    {
        uint8_t value;

        asm volatile(
            "inb %1, %0"
            : "=a"(value)
            : "Nd"(port)
        );

        return value;
    }

    void ioWait()
    {
        outb(0x80, 0);
    }
}

namespace PIC
{
    void initialize()
    {

        uint8_t masterMask = inb(PIC1_DATA);
        uint8_t slaveMask  = inb(PIC2_DATA);

        outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
        ioWait();

        outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
        ioWait();

        outb(PIC1_DATA, 0x20);
        ioWait();

        outb(PIC2_DATA, 0x28);
        ioWait();

        outb(PIC1_DATA, 0x04);
        ioWait();

        outb(PIC2_DATA, 0x02);
        ioWait();

        outb(PIC1_DATA, ICW4_8086);
        ioWait();

        outb(PIC2_DATA, ICW4_8086);
        ioWait();

        outb(PIC1_DATA, 0xFF);
        outb(PIC2_DATA, 0xFF);

        (void)masterMask;
        (void)slaveMask;
    }

    void sendEndOfInterrupt(uint8_t irq)
    {
        if (irq >= 8)
        {
            outb(PIC2_COMMAND, 0x20);
        }

        outb(PIC1_COMMAND, 0x20);
    }

    void mask(uint8_t irq)
    {
        uint16_t port;

        if (irq < 8)
            port = PIC1_DATA;
        else
        {
            port = PIC2_DATA;
            irq -= 8;
        }

        uint8_t value = inb(port);

        value |= (1 << irq);

        outb(port, value);
    }

    void unmask(uint8_t irq)
    {
        uint16_t port;

        if (irq < 8)
            port = PIC1_DATA;
        else
        {
            port = PIC2_DATA;
            irq -= 8;
        }

        uint8_t value = inb(port);

        value &= ~(1 << irq);

        outb(port, value);
    }
}