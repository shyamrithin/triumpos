/*
 * ============================================================
 * Triumph OS - ISR Header
 * File: kernel/isr.h
 * Description: Declares interrupt handler types and functions
 *              used by drivers to register IRQ handlers.
 * ============================================================
 */

#ifndef ISR_H
#define ISR_H

#include "idt.h"

/* Function pointer type for IRQ handlers */
typedef void (*irq_handler_t)(registers_t regs);

/* Register a handler for a hardware IRQ line (0-15) */
void irq_register(uint8_t irq, irq_handler_t handler);

/* Called by assembly — do not call directly */
void isr_handler(registers_t regs);
void irq_handler(registers_t regs);

#endif