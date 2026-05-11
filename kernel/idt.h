/*
 * ============================================================
 * Triumph OS - IDT Header
 * File: kernel/idt.h
 * Description: Defines the Interrupt Descriptor Table
 *              structures and declarations. The IDT tells the
 *              CPU which function to call for each interrupt
 *              or exception that occurs.
 * ============================================================
 */

#ifndef IDT_H
#define IDT_H

#include <stdint.h>

/*
 * IDT Entry (Gate Descriptor) — 8 bytes each
 *
 *  63       48 47  44 43  40 39  32 31      16 15       0
 * ┌──────────┬───┬────┬────────┬──────────┬──────────┐
 * │ offset   │ P │DPL │  type  │ selector │ offset   │
 * │ [31:16]  │   │    │        │          │ [15:0]   │
 * └──────────┴───┴────┴────────┴──────────┴──────────┘
 */
typedef struct {
    uint16_t offset_low;    /* Lower 16 bits of handler address  */
    uint16_t selector;      /* Kernel code segment selector      */
    uint8_t  zero;          /* Always zero — reserved by CPU     */
    uint8_t  type_attr;     /* Type and attributes               */
    uint16_t offset_high;   /* Upper 16 bits of handler address  */
} __attribute__((packed)) idt_entry_t;

/*
 * IDT Pointer — passed to the lidt instruction
 * Same concept as GDT pointer
 */
typedef struct {
    uint16_t limit;         /* Size of IDT in bytes - 1          */
    uint32_t base;          /* Physical address of IDT           */
} __attribute__((packed)) idt_ptr_t;

/* IDT Gate Types */
#define IDT_GATE_INTERRUPT  0x8E    /* Present | Ring0 | 32-bit interrupt gate */
#define IDT_GATE_TRAP       0x8F    /* Present | Ring0 | 32-bit trap gate      */

/* Total number of IDT entries */
#define IDT_ENTRY_COUNT     256

/*
 * CPU pushed registers — saved on stack by ISR stubs
 * Matches the layout pushed by our assembly ISR stubs
 */
typedef struct {
    uint32_t ds;                        /* Data segment                  */
    uint32_t edi, esi, ebp, esp;        /* Pushed by pusha               */
    uint32_t ebx, edx, ecx, eax;        /* Pushed by pusha               */
    uint32_t int_no, err_code;          /* Interrupt number + error code */
    uint32_t eip, cs, eflags;           /* Pushed by CPU automatically   */
    uint32_t useresp, ss;               /* Pushed by CPU on ring switch  */
} registers_t;

/* Function declarations */
void idt_init(void);
void idt_set_gate(uint8_t num, uint32_t handler, uint16_t selector, uint8_t type_attr);

#endif