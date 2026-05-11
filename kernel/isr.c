/*
 * ============================================================
 * Triumph OS - Interrupt Service Routines
 * File: kernel/isr.c
 * Description: C handlers for CPU exceptions and hardware
 *              IRQs. isr_handler() handles CPU faults.
 *              irq_handler() handles hardware interrupts.
 *              Called from assembly stubs in idt_flush.asm.
 * ============================================================
 */

#include "isr.h"
#include "idt.h"
#include <stdint.h>

/* ── Exception name table ───────────────────────────────────── */
static const char* exception_names[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 FPU Error",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating Point Exception",
    "Virtualization Exception",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved", "Reserved", "Reserved", "Reserved",
    "Reserved",
    "Security Exception",
    "Reserved"
};

/* ── IRQ handler function pointers ──────────────────────────── */
/* Any driver can register itself here to handle a specific IRQ */
static irq_handler_t irq_handlers[16] = { 0 };

/*
 * isr_handler()
 * Called by assembly stub when a CPU exception fires.
 * Prints exception info and halts — exceptions are fatal
 * for now until we have proper error recovery.
 */
void isr_handler(registers_t regs) {
    /* We need vga_print — forward declare here for now */
    extern void vga_print(const char* str);
    extern void vga_print_color(const char* str, int fg, int bg);

    vga_print_color("\n[EXCEPTION] ", 12, 0);   /* Light red */
    vga_print(exception_names[regs.int_no]);
    vga_print(" (interrupt ");

    /* Print interrupt number manually — no printf yet */
    char num[3];
    num[0] = '0' + (regs.int_no / 10);
    num[1] = '0' + (regs.int_no % 10);
    num[2] = '\0';
    vga_print(num);
    vga_print(")\n");

    vga_print_color("System halted.\n", 12, 0);

    /* Halt forever — exception is unrecoverable for now */
    for (;;) {}
}

/*
 * irq_register()
 * Lets a driver register a handler for a specific IRQ line.
 * For example: irq_register(1, keyboard_handler)
 */
void irq_register(uint8_t irq, irq_handler_t handler) {
    irq_handlers[irq] = handler;
}

/*
 * irq_handler()
 * Called by assembly stub when a hardware IRQ fires.
 * Sends EOI to PIC, then calls registered driver handler.
 */
void irq_handler(registers_t regs) {
    uint8_t irq = regs.int_no - 32;    /* Convert back to IRQ number */

    /*
     * Send End of Interrupt (EOI) to PIC.
     * Without this, the PIC thinks we're still handling the
     * interrupt and won't send any more. The OS goes deaf.
     *
     * If IRQ >= 8 it came from the slave PIC (0xA0),
     * so we must notify both slave and master (0x20).
     */
    if (irq >= 8) {
        __asm__ volatile("outb %0, %1" :: "a"((uint8_t)0x20), "Nd"((uint16_t)0xA0));
    }
    __asm__ volatile("outb %0, %1" :: "a"((uint8_t)0x20), "Nd"((uint16_t)0x20));

    /* Call the registered handler if one exists */
    if (irq_handlers[irq]) {
        irq_handlers[irq](regs);
    }
}