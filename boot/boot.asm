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
global idt_load

global isr0
global isr1
global isr2
global isr3
global isr4
global isr5
global isr6
global isr7
global isr8
global isr9
global isr10
global isr11
global isr12
global isr13
global isr14
global isr15
global isr16
global isr17
global isr18
global isr19
global isr20
global isr21
global isr22
global isr23
global isr24
global isr25
global isr26
global isr27
global isr28
global isr29
global isr30
global isr31

; Hardware IRQs
global irq0
global irq1
global irq2
global irq3
global irq4
global irq5
global irq6
global irq7
global irq8
global irq9
global irq10
global irq11
global irq12
global irq13
global irq14
global irq15

extern exception_handler
extern timer_handler
extern kernel_main

_start:
    cli
    mov esp, stack_top

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

section .text
bits 64

idt_load:
    lidt [rdi]
    ret

isr_common:

    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15


    mov rdi, [rsp + 120]
    mov rsi, [rsp + 128]

    call exception_handler

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 16

    iretq


isr0:
    push qword 0
    push qword 0
    jmp isr_common

isr1:
    push qword 0
    push qword 1
    jmp isr_common

isr2:
    push qword 0
    push qword 2
    jmp isr_common

isr3:
    push qword 0
    push qword 3
    jmp isr_common

isr4:
    push qword 0
    push qword 4
    jmp isr_common

isr5:
    push qword 0
    push qword 5
    jmp isr_common

isr6:
    push qword 0
    push qword 6
    jmp isr_common

isr7:
    push qword 0
    push qword 7
    jmp isr_common

; CPU supplies error code
isr8:
    push qword 8
    jmp isr_common

isr9:
    push qword 0
    push qword 9
    jmp isr_common

; CPU supplies error code
isr10:
    push qword 10
    jmp isr_common

; CPU supplies error code
isr11:
    push qword 11
    jmp isr_common

; CPU supplies error code
isr12:
    push qword 12
    jmp isr_common

; CPU supplies error code
isr13:
    push qword 13
    jmp isr_common

; CPU supplies error code
isr14:
    push qword 14
    jmp isr_common

isr15:
    push qword 0
    push qword 15
    jmp isr_common

isr16:
    push qword 0
    push qword 16
    jmp isr_common

; CPU supplies error code
isr17:
    push qword 17
    jmp isr_common

isr18:
    push qword 0
    push qword 18
    jmp isr_common

isr19:
    push qword 0
    push qword 19
    jmp isr_common

isr20:
    push qword 0
    push qword 20
    jmp isr_common

isr21:
    push qword 0
    push qword 21
    jmp isr_common

isr22:
    push qword 0
    push qword 22
    jmp isr_common

isr23:
    push qword 0
    push qword 23
    jmp isr_common

isr24:
    push qword 0
    push qword 24
    jmp isr_common

isr25:
    push qword 0
    push qword 25
    jmp isr_common

isr26:
    push qword 0
    push qword 26
    jmp isr_common

isr27:
    push qword 0
    push qword 27
    jmp isr_common

isr28:
    push qword 0
    push qword 28
    jmp isr_common

isr29:
    push qword 0
    push qword 29
    jmp isr_common

isr30:
    push qword 0
    push qword 30
    jmp isr_common

isr31:
    push qword 0
    push qword 31
    jmp isr_common


; ============================================================
; HARDWARE IRQ COMMON HANDLER
; ============================================================

irq_common:

    push rax
    push rbx
    push rcx
    push rdx
    push rsi
    push rdi
    push rbp
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15

    mov rax, [rsp + 120]

    cmp rax, 32
    jne .not_timer

    call timer_handler

.not_timer:

    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rbp
    pop rdi
    pop rsi
    pop rdx
    pop rcx
    pop rbx
    pop rax

    add rsp, 8

    iretq


irq0:
    push qword 32
    jmp irq_common

irq1:
    push qword 33
    jmp irq_common

irq2:
    push qword 34
    jmp irq_common

irq3:
    push qword 35
    jmp irq_common

irq4:
    push qword 36
    jmp irq_common

irq5:
    push qword 37
    jmp irq_common

irq6:
    push qword 38
    jmp irq_common

irq7:
    push qword 39
    jmp irq_common

irq8:
    push qword 40
    jmp irq_common

irq9:
    push qword 41
    jmp irq_common

irq10:
    push qword 42
    jmp irq_common

irq11:
    push qword 43
    jmp irq_common

irq12:
    push qword 44
    jmp irq_common

irq13:
    push qword 45
    jmp irq_common

irq14:
    push qword 46
    jmp irq_common

irq15:
    push qword 47
    jmp irq_common