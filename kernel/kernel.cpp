#include "console.hpp"
#include "gdt.hh"
#include "idt.hh"
#include "keyboard.hpp"
#include "pic.hpp"
#include "memory/heap.hpp"
#include "memory/physical.hpp"
#include "pit.hpp"
#include "shell/shell.hpp"

extern "C"
char kernel_end;

extern "C"
void kernel_main(uintptr_t multibootInfo)
{
    asm volatile("cli");

    Console::clear();

    Console::setColor(0x0F);

    Console::writeLine(
        "========================================"
    );

    Console::writeLine(
        "          KALER OS v0.5"
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

    PhysicalMemory::initialize(
        multibootInfo,
        reinterpret_cast<uintptr_t>(&kernel_end)
    );

    Heap::initialize();
    PIT::initialize(100);
    Keyboard::initialize();

    Console::writeLine("");

    Console::writeLine(
        "Interrupts: ready"
    );

    Console::writeLine("");

    Console::writeLine(
        "Enabling interrupts..."
    );

    asm volatile("sti");

    Console::writeLine(
        "Interrupts enabled."
    );

    Console::writeLine("");

    Shell::initialize();

    while (true)
    {
        asm volatile("hlt");
    }
}