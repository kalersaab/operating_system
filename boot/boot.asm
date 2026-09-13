bits 32

section .multiboot
align 8
multiboot_header:
    dd 0xE85250D6
    dd 0
    dd multiboot_header_end - multiboot_header
    dd -(0xE85250D6 + 0 + (multiboot_header_end - multiboot_header))
    dw 0
    dw 0
    dd 8
multiboot_header_end:

section .text

global _start

extern kernel_main

_start:
    cli
    mov esp, stack_top
    mov esi, ebx

    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz boot_halt

    mov eax, pdpt_table
    or eax, 0x3
    mov [pml4_table], eax

    mov eax, page_directory
    or eax, 0x3
    mov [pdpt_table], eax

    mov eax, 0x83
    mov [page_directory], eax

    mov eax, 0x200083
    mov [page_directory + 8], eax

    mov eax, 0x400083
    mov [page_directory + 16], eax

    mov eax, 0x600083
    mov [page_directory + 24], eax

    mov eax, pml4_table
    mov cr3, eax

    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    lgdt [boot_gdt.pointer]
    jmp 0x08:long_mode_start

boot_halt:
    hlt
    jmp boot_halt

bits 64

long_mode_start:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov rsp, stack_top
    xor ebp, ebp
    mov edi, esi
    call kernel_main
    jmp boot_halt

section .data
align 8
boot_gdt:
    dq 0
    dq 0x00AF9A000000FFFF
    dq 0x00CF92000000FFFF
.pointer:
    dw .pointer - boot_gdt - 1
    dd boot_gdt

section .bss
align 4096
pml4_table:
    resb 4096
pdpt_table:
    resb 4096
page_directory:
    resb 4096
align 16
stack_bottom:
    resb 16384
stack_top: