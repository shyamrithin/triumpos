; ============================================================
; Triumph OS - GDT Flush
; File: boot/gdt_flush.asm
; Description: Loads the GDT into the CPU using the lgdt
;              instruction, then reloads all segment registers
;              to make the new GDT take effect immediately.
; ============================================================

section .text
global gdt_flush        ; Make visible to C code

; void gdt_flush(uint32_t gdt_ptr_address)
; Parameter: passed on the stack by C calling convention
gdt_flush:
    mov eax, [esp + 4]  ; Get gdt_ptr address from stack
    lgdt [eax]          ; Load GDT into CPU

    ; Reload segment registers with kernel data selector
    ; 0x10 = index 2 (kernel data) in GDT × 8 bytes = 16 = 0x10
    mov ax, 0x10
    mov ds, ax          ; Data segment
    mov es, ax          ; Extra segment
    mov fs, ax          ; General purpose segment
    mov gs, ax          ; General purpose segment
    mov ss, ax          ; Stack segment

    ; Reload CS (code segment) with kernel code selector
    ; 0x08 = index 1 (kernel code) in GDT × 8 bytes = 8 = 0x08
    ; CS cannot be set with mov — must use a far jump
    jmp 0x08:.flush

.flush:
    ret                 ; Return to C kernel