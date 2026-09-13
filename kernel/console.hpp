#pragma once

#include <stdint.h>

namespace Console
{
    void clear();

    void putChar(char c);

    void write(const char* str);

    void writeLine(const char* str);

    void writeHex(uint64_t value);

    void writeDec(uint64_t value);

    void setColor(uint8_t color);
}