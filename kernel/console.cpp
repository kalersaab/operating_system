#include "console.hpp"

namespace
{
    volatile uint16_t* const VGA_MEMORY =
        reinterpret_cast<volatile uint16_t*>(0xB8000);

    constexpr uint16_t VGA_WIDTH = 80;
    constexpr uint16_t VGA_HEIGHT = 25;

    uint16_t cursorRow = 0;
    uint16_t cursorColumn = 0;

    uint8_t color = 0x07;

    void updateCursor()
    {
        uint16_t position =
            cursorRow * VGA_WIDTH + cursorColumn;

        asm volatile(
            "outb %0, %1"
            :
            : "a"(static_cast<uint8_t>(0x0F)), "Nd"(static_cast<uint16_t>(0x3D4))
        );

        asm volatile(
            "outb %0, %1"
            :
            : "a"(static_cast<uint8_t>(position & 0xFF)), "Nd"(static_cast<uint16_t>(0x3D5))
        );

        asm volatile(
            "outb %0, %1"
            :
            : "a"(static_cast<uint8_t>(0x0E)), "Nd"(static_cast<uint16_t>(0x3D4))
        );

        asm volatile(
            "outb %0, %1"
            :
            : "a"(static_cast<uint8_t>(position >> 8)), "Nd"(static_cast<uint16_t>(0x3D5))
        );
    }

    uint16_t makeEntry(char c, uint8_t colour)
    {
        return static_cast<uint16_t>(c)
             | (static_cast<uint16_t>(colour) << 8);
    }

    void scroll()
    {
        if (cursorRow < VGA_HEIGHT)
            return;

        for (uint16_t row = 1; row < VGA_HEIGHT; ++row)
        {
            for (uint16_t col = 0; col < VGA_WIDTH; ++col)
            {
                VGA_MEMORY[(row - 1) * VGA_WIDTH + col] =
                    VGA_MEMORY[row * VGA_WIDTH + col];
            }
        }

        for (uint16_t col = 0; col < VGA_WIDTH; ++col)
        {
            VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + col] =
                makeEntry(' ', color);
        }

        cursorRow = VGA_HEIGHT - 1;
    }
}

namespace Console
{
    void clear()
    {
        for (uint16_t row = 0; row < VGA_HEIGHT; ++row)
        {
            for (uint16_t col = 0; col < VGA_WIDTH; ++col)
            {
                VGA_MEMORY[row * VGA_WIDTH + col] =
                    makeEntry(' ', color);
            }
        }

        cursorRow = 0;
        cursorColumn = 0;
        updateCursor();
    }

    void putChar(char c)
    {
        if (c == '\n')
        {
            cursorColumn = 0;
            ++cursorRow;

            scroll();
            updateCursor();
            return;
        }

        if (c == '\r')
        {
            cursorColumn = 0;
            updateCursor();
            return;
        }
        if (c == '\b')
{
    if (cursorColumn > 0)
    {
        --cursorColumn;

        VGA_MEMORY[
            cursorRow * VGA_WIDTH + cursorColumn
        ] = makeEntry(' ', color);
    }

    updateCursor();
    return;
}
        VGA_MEMORY[cursorRow * VGA_WIDTH + cursorColumn] =
            makeEntry(c, color);

        ++cursorColumn;

        if (cursorColumn >= VGA_WIDTH)
        {
            cursorColumn = 0;
            ++cursorRow;
        }

        scroll();
        updateCursor();
    }

    void write(const char* str)
    {
        if (!str)
            return;

        while (*str)
        {
            putChar(*str);
            ++str;
        }
    }

    void writeLine(const char* str)
    {
        write(str);
        putChar('\n');
    }

    void writeHex(uint64_t value)
    {
        const char* hex = "0123456789ABCDEF";

        write("0x");

        for (int i = 15; i >= 0; --i)
        {
            putChar(hex[(value >> (i * 4)) & 0xF]);
        }
    }

    void writeDec(uint64_t value)
    {
        if (value == 0)
        {
            putChar('0');
            return;
        }

        char buffer[21];
        int index = 0;

        while (value > 0)
        {
            buffer[index++] =
                static_cast<char>('0' + (value % 10));

            value /= 10;
        }

        while (index > 0)
        {
            putChar(buffer[--index]);
        }
    }

    void setColor(uint8_t newColor)
    {
        color = newColor;
    }
}