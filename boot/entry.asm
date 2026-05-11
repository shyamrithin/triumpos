; ============================================================
; Triumph OS - Kernel Entry Point
; File: boot/entry.asm
; Description: Multiboot2-compliant assembly entry point.
;              Sets up the stack and hands control to the
;              C kernel. This is the FIRST code Triumph OS
;              ever executes.
; ============================================================

; --- Constants ---
MBOOT_MAGIC     equ 0xe85250d6      ; Multiboot2 magic number GRUB looks for
MBOOT_ARCH      equ 0               ; 0 = x86 protected mode
MBOOT_LENGTH    equ (mboot_end - mboot_start)
MBOOT_CHECKSUM  equ -(MBOOT_MAGIC + MBOOT_ARCH + MBOOT_LENGTH)

; --- Multiboot2 Header ---
section .multiboot
align 8
mboot_start:
    dd MBOOT_MAGIC
    dd MBOOT_ARCH
    dd MBOOT_LENGTH
    dd MBOOT_CHECKSUM & 0xFFFFFFFF
    ; End tag - tells GRUB there are no more tags
    dw 0
    dw 0
    dd 8
mboot_end:

; --- Stack ---
section .bss
align 16
stack_bottom:
    resb 16384          ; Reserve 16KB for the stack
stack_top:

; --- Entry Point ---
section .text
global _start           ; Make _start visible to the linker
extern kernel_main      ; kernel_main is defined in kernel.c

_start:
    mov esp, stack_top  ; Point stack pointer to top of our stack
    call kernel_main    ; Call C kernel
    cli                 ; Disable interrupts if kernel returns
.hang:
    hlt                 ; Halt the CPU
    jmp .hang           ; Loop forever just in case