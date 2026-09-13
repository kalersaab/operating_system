#include "shell.hpp"

#include "console.hpp"
#include "memory/heap.hpp"
#include "memory/physical.hpp"
#include "pit.hpp"

#include <stdint.h>

namespace
{
    char commandLine[128];
    uint8_t commandLength = 0;

    bool equals(const char* left, const char* right)
    {
        while (*left && *right && *left == *right)
        {
            ++left;
            ++right;
        }

        return *left == 0 && *right == 0;
    }

    bool startsWith(const char* value, const char* prefix)
    {
        while (*prefix)
        {
            if (*value++ != *prefix++)
                return false;
        }

        return true;
    }

    void prompt()
    {
        Console::write("kernel> ");
    }

    [[noreturn]] void halt()
    {
        while (true)
            asm volatile("cli; hlt");
    }

    [[noreturn]] void shutdown()
    {
        asm volatile(
            "outw %0, %1"
            :
            : "a"(static_cast<uint16_t>(0x2000)),
              "Nd"(static_cast<uint16_t>(0x604))
        );

        asm volatile(
            "outw %0, %1"
            :
            : "a"(static_cast<uint16_t>(0x2000)),
              "Nd"(static_cast<uint16_t>(0xB004))
        );

        halt();
    }

    [[noreturn]] void reboot()
    {
        uint8_t status;

        do
        {
            asm volatile(
                "inb %1, %0"
                : "=a"(status)
                : "Nd"(static_cast<uint16_t>(0x64))
            );
        }
        while (status & 0x02);

        asm volatile(
            "outb %0, %1"
            :
            : "a"(static_cast<uint8_t>(0xFE)),
              "Nd"(static_cast<uint16_t>(0x64))
        );

        halt();
    }
}

namespace Shell
{
    void initialize()
    {
        prompt();
    }

    void handleCharacter(char character)
    {
        if (commandLength >= sizeof(commandLine) - 1)
            return;

        commandLine[commandLength++] = character;
        Console::putChar(character);
    }

    void handleBackspace()
    {
        if (commandLength == 0)
            return;

        --commandLength;
        Console::putChar('\b');
    }

    void submit()
    {
        commandLine[commandLength] = 0;
        Console::putChar('\n');
        handleCommand(commandLine);
        commandLength = 0;
    }

    void handleCommand(const char* command)
    {
        if (equals(command, "help"))
        {
            Console::writeLine("Available commands:");
            Console::writeLine("  help   list commands");
            Console::writeLine("  clear  clear the screen");
            Console::writeLine("  info   show kernel information");
            Console::writeLine("  ticks  show timer ticks");
            Console::writeLine("  echo   print text");
            Console::writeLine("  reboot reboot the system");
            Console::writeLine("  shutdown power off the system");
        }
        else if (equals(command, "clear") || equals(command, "cls"))
        {
            Console::clear();
        }
        else if (equals(command, "info"))
        {
            Console::writeLine("KalerOS v0.5");
            Console::writeLine("Architecture: x86-64");
            Console::write("Physical frames: ");
            Console::writeDec(PhysicalMemory::usedFrames());
            Console::write(" / ");
            Console::writeDec(PhysicalMemory::totalFrames());
            Console::writeLine("");
            Console::write("Heap bytes: ");
            Console::writeDec(Heap::usedBytes());
            Console::writeLine("");
        }
        else if (equals(command, "ticks"))
        {
            Console::write("Timer ticks: ");
            Console::writeDec(PIT::getTicks());
            Console::writeLine("");
        }
        else if (startsWith(command, "echo "))
        {
            Console::writeLine(command + 5);
        }
        else if (equals(command, "reboot"))
        {
            Console::writeLine("Rebooting...");
            reboot();
        }
        else if (equals(command, "shutdown"))
        {
            Console::writeLine("Shutting down...");
            shutdown();
        }
        else if (command[0] != 0)
        {
            Console::write("Unknown command: ");
            Console::writeLine(command);
        }

        prompt();
    }
}
