#include "console.hpp"
#include "gdt.hh"
#include "idt.hh"

extern "C"
void kernel_main()
{
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

    Console::write("GDT:        ");

    GDT::initialize();

    Console::writeLine("OK");

    Console::write("IDT:        ");

    IDT::initialize();

    Console::writeLine("OK");

    Console::writeLine("");

    Console::writeLine(
        "Exceptions: installed"
    );

    Console::writeLine("");

    Console::writeLine(
        "Interrupt system ready."
    );

    Console::writeLine("");

    Console::writeLine(
        "kernel> _"
    );

    while (true)
    {
        asm volatile("hlt");
    }
}