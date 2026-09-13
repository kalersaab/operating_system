#pragma once

namespace Shell
{
    void initialize();
    void handleCharacter(char character);
    void handleBackspace();
    void submit();
    void handleCommand(const char* command);
}
