/*
 * ============================================================
 * Triumph OS - GDT Header
 * File: kernel/gdt.h
 * Description: Defines the Global Descriptor Table structures
 *              and function declarations. The GDT tells the
 *              CPU about memory segments and their permissions.
 * ============================================================
 */

#ifndef GDT_H
#define GDT_H

#include <stdint.h>

/*
 * GDT Entry (Segment Descriptor) — 8 bytes each
 *
 * Each entry describes one memory segment:
 *
 *  63       56 55   52 51  48 47    40 39      16 15       0
 * ┌──────────┬───────┬──────┬────────┬──────────┬──────────┐
 * │ base     │ flags │limit │ access │ base     │  limit   │
 * │ [31:24]  │       │[19:16]│       │ [23:0]   │ [15:0]   │
 * └──────────┴───────┴──────┴────────┴──────────┴──────────┘
 */
typedef struct {
    uint16_t limit_low;     /* Lower 16 bits of segment limit    */
    uint16_t base_low;      /* Lower 16 bits of base address     */
    uint8_t  base_mid;      /* Middle 8 bits of base address     */
    uint8_t  access;        /* Access flags — permissions        */
    uint8_t  granularity;   /* Limit high bits + flags           */
    uint8_t  base_high;     /* Upper 8 bits of base address      */
} __attribute__((packed)) gdt_entry_t;

/*
 * GDT Pointer — passed to the lgdt instruction
 * Tells the CPU where the GDT is in memory and how big it is
 */
typedef struct {
    uint16_t limit;         /* Size of GDT in bytes - 1          */
    uint32_t base;          /* Physical address of GDT           */
} __attribute__((packed)) gdt_ptr_t;

/* GDT Segment Indices */
#define GDT_NULL_SEGMENT    0
#define GDT_KERNEL_CODE     1
#define GDT_KERNEL_DATA     2
#define GDT_ENTRY_COUNT     3

/* Access byte flags */
#define GDT_ACCESS_PRESENT      (1 << 7)  /* Segment is present in memory  */
#define GDT_ACCESS_RING0        (0 << 5)  /* Privilege level 0 (kernel)    */
#define GDT_ACCESS_DESCRIPTOR   (1 << 4)  /* Code/data segment (not system)*/
#define GDT_ACCESS_EXECUTABLE   (1 << 3)  /* Executable (code segment)     */
#define GDT_ACCESS_READWRITE    (1 << 1)  /* Readable/writable             */

/* Granularity byte flags */
#define GDT_FLAG_32BIT          (1 << 6)  /* 32-bit protected mode         */
#define GDT_FLAG_4K_GRAN        (1 << 7)  /* Limit in 4KB blocks           */

/* Function declarations */
void gdt_init(void);

#endif