; ============================================================
; Triumph OS - IDT Flush + ISR/IRQ Stubs
; File: boot/idt_flush.asm
; Description: Loads the IDT into the CPU. Defines assembly
;              stubs for all 32 CPU exceptions and 16 hardware
;              IRQs. Each stub saves CPU state, calls the C
;              handler, then restores state and returns.
; ============================================================

section .text

; ── Load IDT into CPU ────────────────────────────────────────
global idt_flush
idt_flush:
    mov eax, [esp + 4]      ; Get idt_ptr address from stack
    lidt [eax]              ; Load IDT into CPU's IDTR register
    ret

; ── Macro: Exception WITH error code ─────────────────────────
; Some CPU exceptions push an error code automatically.
; For those we just push the interrupt number and jump.
%macro ISR_ERRCODE 1
global isr%1
isr%1:
    push dword %1           ; Push interrupt number
    jmp isr_common          ; Jump to common handler
%endmacro

; ── Macro: Exception WITHOUT error code ──────────────────────
; Most exceptions don't push an error code.
; We push a dummy 0 so the stack layout is always the same.
%macro ISR_NOERRCODE 1
global isr%1
isr%1:
    push dword 0            ; Push dummy error code
    push dword %1           ; Push interrupt number
    jmp isr_common          ; Jump to common handler
%endmacro

; ── Macro: Hardware IRQ stub ──────────────────────────────────
; Hardware interrupts never push an error code.
; We push 0 as error code and the IRQ number remapped to 32+.
%macro IRQ 2
global irq%1
irq%1:
    push dword 0            ; Dummy error code
    push dword %2           ; Interrupt number (32 + IRQ number)
    jmp irq_common          ; Jump to common IRQ handler
%endmacro

; ── CPU Exception stubs (0-31) ────────────────────────────────
; Intel manual defines which exceptions push error codes:
ISR_NOERRCODE  0    ; Division by zero
ISR_NOERRCODE  1    ; Debug
ISR_NOERRCODE  2    ; Non-maskable interrupt
ISR_NOERRCODE  3    ; Breakpoint
ISR_NOERRCODE  4    ; Overflow
ISR_NOERRCODE  5    ; Bound range exceeded
ISR_NOERRCODE  6    ; Invalid opcode
ISR_NOERRCODE  7    ; Device not available
ISR_ERRCODE    8    ; Double fault          (pushes error code)
ISR_NOERRCODE  9    ; Coprocessor segment overrun
ISR_ERRCODE   10    ; Invalid TSS           (pushes error code)
ISR_ERRCODE   11    ; Segment not present   (pushes error code)
ISR_ERRCODE   12    ; Stack segment fault   (pushes error code)
ISR_ERRCODE   13    ; General protection    (pushes error code)
ISR_ERRCODE   14    ; Page fault            (pushes error code)
ISR_NOERRCODE 15    ; Reserved
ISR_NOERRCODE 16    ; x87 FPU error
ISR_ERRCODE   17    ; Alignment check       (pushes error code)
ISR_NOERRCODE 18    ; Machine check
ISR_NOERRCODE 19    ; SIMD floating point
ISR_NOERRCODE 20    ; Virtualization
ISR_NOERRCODE 21    ; Reserved
ISR_NOERRCODE 22    ; Reserved
ISR_NOERRCODE 23    ; Reserved
ISR_NOERRCODE 24    ; Reserved
ISR_NOERRCODE 25    ; Reserved
ISR_NOERRCODE 26    ; Reserved
ISR_NOERRCODE 27    ; Reserved
ISR_NOERRCODE 28    ; Reserved
ISR_NOERRCODE 29    ; Reserved
ISR_ERRCODE   30    ; Security exception    (pushes error code)
ISR_NOERRCODE 31    ; Reserved

; ── Hardware IRQ stubs (0-15) ─────────────────────────────────
; IRQ 0-7  are mapped to interrupts 32-39
; IRQ 8-15 are mapped to interrupts 40-47
IRQ  0, 32      ; Timer
IRQ  1, 33      ; Keyboard
IRQ  2, 34      ; Cascade (used internally by PICs)
IRQ  3, 35      ; COM2
IRQ  4, 36      ; COM1
IRQ  5, 37      ; LPT2
IRQ  6, 38      ; Floppy disk
IRQ  7, 39      ; LPT1
IRQ  8, 40      ; CMOS real-time clock
IRQ  9, 41      ; Free / legacy SCSI / NIC
IRQ 10, 42      ; Free / SCSI / NIC
IRQ 11, 43      ; Free / SCSI / NIC
IRQ 12, 44      ; PS/2 mouse
IRQ 13, 45      ; FPU / coprocessor
IRQ 14, 46      ; Primary ATA hard disk
IRQ 15, 47      ; Secondary ATA hard disk

; ── Common ISR handler (CPU exceptions) ──────────────────────
; At this point the stack looks like:
;   [esp+0]  = interrupt number  (pushed by our stub)
;   [esp+4]  = error code        (pushed by CPU or dummy 0)
;   [esp+8]  = eip               (pushed by CPU)
;   [esp+12] = cs                (pushed by CPU)
;   [esp+16] = eflags            (pushed by CPU)
extern isr_handler
isr_common:
    pusha                   ; Push eax,ecx,edx,ebx,esp,ebp,esi,edi
    mov ax, ds              ; Save data segment
    push eax

    mov ax, 0x10            ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call isr_handler        ; Call C exception handler

    pop eax                 ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                    ; Restore all registers
    add esp, 8              ; Clean up error code + int number
    iret                    ; Return from interrupt

; ── Common IRQ handler (hardware interrupts) ─────────────────
extern irq_handler
irq_common:
    pusha                   ; Save all registers
    mov ax, ds
    push eax

    mov ax, 0x10            ; Load kernel data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    call irq_handler        ; Call C IRQ handler

    pop eax                 ; Restore data segment
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    popa                    ; Restore all registers
    add esp, 8              ; Clean up error code + int number
    iret                    ; Return from interrupt