#include "idt.hh"
#include "console.hpp"

#include <stdint.h>

extern "C"
{
    void idt_load(uint64_t idtAddress);

    void isr0();
    void isr1();
    void isr2();
    void isr3();
    void isr4();
    void isr5();
    void isr6();
    void isr7();
    void isr8();
    void isr9();
    void isr10();
    void isr11();
    void isr12();
    void isr13();
    void isr14();
    void isr15();
    void isr16();
    void isr17();
    void isr18();
    void isr19();
    void isr20();
    void isr21();
    void isr22();
    void isr23();
    void isr24();
    void isr25();
    void isr26();
    void isr27();
    void isr28();
    void isr29();
    void isr30();
    void isr31();

    void irq0();
    void irq1();
    void irq2();
    void irq3();
    void irq4();
    void irq5();
    void irq6();
    void irq7();
    void irq8();
    void irq9();
    void irq10();
    void irq11();
    void irq12();
    void irq13();
    void irq14();
    void irq15();
}

namespace
{
    struct [[gnu::packed]] IDTEntry
    {
        uint16_t offsetLow;
        uint16_t selector;

        uint8_t ist;
        uint8_t typeAttributes;

        uint16_t offsetMiddle;

        uint32_t offsetHigh;
        uint32_t reserved;
    };

    struct [[gnu::packed]] IDTPointer
    {
        uint16_t limit;
        uint64_t base;
    };

    IDTEntry idt[256];
    IDTPointer idtPointer;

    using ISR = void (*)();

    void setGate(
        uint8_t vector,
        ISR handler)
    {
        uint64_t address =
            reinterpret_cast<uint64_t>(handler);

        idt[vector].offsetLow =
            address & 0xFFFF;

        idt[vector].selector =
            0x08;

        idt[vector].ist =
            0;

        idt[vector].typeAttributes =
            0x8E;

        idt[vector].offsetMiddle =
            (address >> 16) & 0xFFFF;

        idt[vector].offsetHigh =
            (address >> 32) & 0xFFFFFFFF;

        idt[vector].reserved = 0;
    }

    void clear()
    {
        for (int i = 0; i < 256; ++i)
        {
            idt[i] = {};
        }
    }
}

namespace IDT
{
    void initialize()
    {
        clear();

        idtPointer.limit =
            sizeof(idt) - 1;

        idtPointer.base =
            reinterpret_cast<uint64_t>(&idt);


        setGate(0, isr0);
        setGate(1, isr1);
        setGate(2, isr2);
        setGate(3, isr3);
        setGate(4, isr4);
        setGate(5, isr5);
        setGate(6, isr6);
        setGate(7, isr7);
        setGate(8, isr8);
        setGate(9, isr9);
        setGate(10, isr10);
        setGate(11, isr11);
        setGate(12, isr12);
        setGate(13, isr13);
        setGate(14, isr14);
        setGate(15, isr15);
        setGate(16, isr16);
        setGate(17, isr17);
        setGate(18, isr18);
        setGate(19, isr19);
        setGate(20, isr20);
        setGate(21, isr21);
        setGate(22, isr22);
        setGate(23, isr23);
        setGate(24, isr24);
        setGate(25, isr25);
        setGate(26, isr26);
        setGate(27, isr27);
        setGate(28, isr28);
        setGate(29, isr29);
        setGate(30, isr30);
        setGate(31, isr31);

        setGate(32, irq0);
        setGate(33, irq1);
        setGate(34, irq2);
        setGate(35, irq3);
        setGate(36, irq4);
        setGate(37, irq5);
        setGate(38, irq6);
        setGate(39, irq7);

        setGate(40, irq8);
        setGate(41, irq9);
        setGate(42, irq10);
        setGate(43, irq11);
        setGate(44, irq12);
        setGate(45, irq13);
        setGate(46, irq14);
        setGate(47, irq15);

        /*
         * Load IDT.
         */
        idt_load(
            reinterpret_cast<uint64_t>(
                &idtPointer
            )
        );
    }
}

extern "C"
void exception_handler(
    uint64_t vector,
    uint64_t errorCode)
{
    Console::writeLine("");

    Console::writeLine(
        "========================================"
    );

    Console::writeLine(
        "             CPU EXCEPTION"
    );

    Console::writeLine(
        "========================================"
    );

    Console::write("Exception vector: ");
    Console::writeDec(vector);
    Console::writeLine("");

    Console::write("Error code:       ");
    Console::writeHex(errorCode);
    Console::writeLine("");

    Console::writeLine("");
    Console::writeLine("KalerOS halted.");

    while (true)
    {
        asm volatile("cli");
        asm volatile("hlt");
    }
}