#include "console.hpp"
#include "gdt.hh"
#include "idt.hh"
#include "pic.hpp"
#include "pit.hpp"

extern "C"
void kernel_main()
{
    asm volatile("cli");

    Console::clear();

    Console::setColor(0x0F);

    Console::writeLine(
        "========================================"
    );

    Console::writeLine(
        "          KALER OS v0.3"
    );

    Console::writeLine(
        "========================================"
    );

    Console::setColor(0x07);

    Console::writeLine("");

    Console::write("CPU:        ");
    Console::writeLine("x86-64");

    /*
     * GDT
     */
    Console::write("GDT:        ");

    GDT::initialize();

    Console::writeLine("OK");

    /*
     * IDT
     */
    Console::write("IDT:        ");

    IDT::initialize();

    Console::writeLine("OK");

    /*
     * PIC
     */
    Console::write("PIC:        ");

    PIC::initialize();

    Console::writeLine("OK");

    /*
     * PIT
     */
    Console::write("PIT:        ");

    PIT::initialize(100);

    Console::writeLine("OK");

    Console::writeLine("");

    Console::writeLine(
        "Interrupts: ready"
    );

    Console::writeLine("");

    Console::write("Timer frequency: ");

    Console::writeDec(100);

    Console::writeLine(" Hz");

    Console::writeLine("");

    Console::writeLine(
        "Enabling interrupts..."
    );

    asm volatile("sti");

    Console::writeLine(
        "Interrupts enabled."
    );

    Console::writeLine("");

    Console::writeLine(
        "kernel> _"
    );
    uint64_t lastTick = 0;
    while (true)
    {
        asm volatile("hlt");
        uint64_t currentTick = PIT::getTicks();

    if (currentTick != lastTick)
    {
        lastTick = currentTick;

        if ((currentTick % 100) == 0)
        {
            Console::write("Timer ticks: ");
            Console::writeDec(currentTick);
            Console::writeLine("");
        }
    }
    }
}