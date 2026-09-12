extern "C" void kernel_main()
{
    volatile unsigned short* video_memory =
        (unsigned short*)0xB8000;

    const char* message =
        "KALER OS v0.1 - Kernel loaded successfully!";

    for (int i = 0; message[i] != '\0'; ++i)
    {
        video_memory[i] =
            (unsigned short)message[i] | (0x07 << 8);
    }

    while (true)
    {
        asm volatile ("hlt");
    }
}