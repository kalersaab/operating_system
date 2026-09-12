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

    ; Set temporary 32-bit stack
    mov esp, stack_top

    ; ------------------------------------------------
    ; Clear page tables
    ; ------------------------------------------------

   mov edi, pml4_table
   xor eax, eax
   mov ecx, 3072
   rep stosd

    ; ------------------------------------------------
    ; PML4[0] -> PDPT
    ; ------------------------------------------------

    mov eax, pdpt_table
    or eax, 0x03
    mov [pml4_table], eax

    ; ------------------------------------------------
    ; PDPT[0] -> Page Directory
    ; ------------------------------------------------

    mov eax, pd_table
    or eax, 0x03
    mov [pdpt_table], eax

    ; ------------------------------------------------
    ; PD[0] = 2 MiB identity mapped page
    ; P = 1
    ; W = 1
    ; PS = 1
    ; ------------------------------------------------

    mov dword [pd_table], 0x00000083

    ; ------------------------------------------------
    ; Load PML4
    ; ------------------------------------------------

    mov eax, pml4_table
    mov cr3, eax

    ; ------------------------------------------------
    ; Enable PAE
    ; CR4.PAE = bit 5
    ; ------------------------------------------------

    mov eax, cr4
    or eax, 0x20
    mov cr4, eax

    ; ------------------------------------------------
    ; Enable Long Mode
    ; EFER MSR = 0xC0000080
    ; LME = bit 8
    ; ------------------------------------------------

    mov ecx, 0xC0000080
    rdmsr

    or eax, 0x100

    wrmsr

    ; ------------------------------------------------
    ; Enable paging
    ; CR0.PG = bit 31
    ; ------------------------------------------------

    mov eax, cr0
    or eax, 0x80000000
    mov cr0, eax

    ; ------------------------------------------------
    ; Load 64-bit GDT
    ; ------------------------------------------------

    lgdt [gdt64_descriptor]

    ; ------------------------------------------------
    ; Far jump into 64-bit code
    ; ------------------------------------------------

    jmp 0x08:long_mode_start


; ====================================================
; 64-bit code
; ====================================================

bits 64

long_mode_start:

    ; Reload segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov ss, ax

    ; 64-bit stack
    mov rsp, stack_top

    ; Call C++ kernel
    mov rax, kernel_main
    call rax

.hang:
    cli
    hlt
    jmp .hang


; ====================================================
; GDT
; ====================================================

align 8

gdt64:

    ; Null descriptor
    dq 0

    ; 64-bit code segment
    dq 0x00AF9A000000FFFF

    ; 64-bit data segment
    dq 0x00AF92000000FFFF

gdt64_end:


gdt64_descriptor:
    dw gdt64_end - gdt64 - 1
    dq gdt64


; ====================================================
; Page tables
; ====================================================

section .bss

align 4096

pml4_table:
    resb 4096

align 4096

pdpt_table:
    resb 4096

align 4096

pd_table:
    resb 4096


; ====================================================
; Stack
; ====================================================

align 16

stack_bottom:
    resb 16384

stack_top: