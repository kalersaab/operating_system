#include "keyboard.hpp"

#include "pic.hpp"
#include "shell/shell.hpp"

#include <stdint.h>

namespace
{
    constexpr uint16_t DATA_PORT = 0x60;
    constexpr uint16_t STATUS_PORT = 0x64;

    bool shiftPressed = false;
    bool capsLock = false;
    bool extendedCode = false;
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

    char scancodeToAscii(uint8_t scanCode)
    {
        static constexpr char normal[] =
        {
            0, 0,
            '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=',
            0, 0,
            'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
            0, 0,
            'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
            0, '\\',
            'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',
            0, '*', 0, ' '
        };

        static constexpr char shifted[] =
        {
            0, 0,
            '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+',
            0, 0,
            'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}',
            0, 0,
            'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',
            0, '|',
            'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',
            0, '*', 0, ' '
        };

        if (scanCode >= sizeof(normal))
            return 0;

        char key = (shiftPressed ? shifted : normal)[scanCode];

        if (capsLock && key >= 'a' && key <= 'z')
            key = static_cast<char>(key - ('a' - 'A'));

        return key;
    }
}

extern "C"
void keyboard_handler()
{
    if ((inb(STATUS_PORT) & 1) == 0)
        return;

    uint8_t scanCode = inb(DATA_PORT);

    if (scanCode == 0xE0)
    {
        extendedCode = true;
        PIC::sendEndOfInterrupt(1);
        return;
    }

    if (extendedCode)
    {
        extendedCode = false;
        PIC::sendEndOfInterrupt(1);
        return;
    }

    bool released = (scanCode & 0x80) != 0;
    scanCode &= 0x7F;

    if (scanCode == 0x2A || scanCode == 0x36)
    {
        shiftPressed = !released;
    }
    else if (!released && scanCode == 0x3A)
    {
        capsLock = !capsLock;
    }
    else if (!released && scanCode == 0x0E)
    {
        Shell::handleBackspace();
    }
    else if (!released && scanCode == 0x1C)
    {
        Shell::submit();
    }
    else if (!released)
    {
        char key = scancodeToAscii(scanCode);

        if (key != 0)
            Shell::handleCharacter(key);
    }

    PIC::sendEndOfInterrupt(1);
}

namespace Keyboard
{
    void initialize()
    {
        while ((inb(STATUS_PORT) & 1) != 0)
            (void)inb(DATA_PORT);

        PIC::unmask(1);
    }
}
