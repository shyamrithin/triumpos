/*
 * ============================================================
 * Triumph OS - GDT Implementation
 * File: kernel/gdt.c
 * Description: Sets up the Global Descriptor Table with
 *              kernel code and data segments. Loads the GDT
 *              into the CPU using the lgdt instruction.
 * ============================================================
 */

#include "gdt.h"

/* The actual GDT — array of segment descriptors */
static gdt_entry_t gdt[GDT_ENTRY_COUNT];

/* GDT pointer passed to lgdt instruction */
static gdt_ptr_t gdt_ptr;

/* Defined in boot/gdt_flush.asm — loads GDT into CPU */
extern void gdt_flush(uint32_t gdt_ptr_address);

/*
 * gdt_set_entry()
 * Fills in one GDT entry with the given parameters.
 * Handles the weird split field layout the CPU expects.
 */
static void gdt_set_entry(int index, uint32_t base, uint32_t limit,
                           uint8_t access, uint8_t granularity) {
    /* Split base address across three fields */
    gdt[index].base_low  = (base & 0xFFFF);
    gdt[index].base_mid  = (base >> 16) & 0xFF;
    gdt[index].base_high = (base >> 24) & 0xFF;

    /* Split limit across two fields */
    gdt[index].limit_low   = (limit & 0xFFFF);
    gdt[index].granularity = (limit >> 16) & 0x0F;

    /* Set flags in granularity byte */
    gdt[index].granularity |= granularity & 0xF0;

    /* Set access byte */
    gdt[index].access = access;
}

/*
 * gdt_init()
 * Sets up the GDT with 3 entries and loads it into the CPU.
 * Called from kernel_main() during boot.
 */
void gdt_init(void) {
    /* Set up the GDT pointer */
    gdt_ptr.limit = (sizeof(gdt_entry_t) * GDT_ENTRY_COUNT) - 1;
    gdt_ptr.base  = (uint32_t)&gdt;

    /*
     * Entry 0: NULL descriptor
     * CPU requires the first entry to be all zeros.
     * Accessing this segment causes a fault — intentional.
     */
    gdt_set_entry(GDT_NULL_SEGMENT, 0, 0, 0, 0);

    /*
     * Entry 1: Kernel Code Segment
     * Base  = 0x00000000 (starts at bottom of memory)
     * Limit = 0xFFFFFFFF (covers entire 4GB address space)
     * Access = present | ring0 | descriptor | executable | readable
     * Flags  = 32-bit | 4KB granularity
     */
    gdt_set_entry(GDT_KERNEL_CODE,
        0x00000000,
        0xFFFFFFFF,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 |
        GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_EXECUTABLE |
        GDT_ACCESS_READWRITE,
        GDT_FLAG_32BIT | GDT_FLAG_4K_GRAN
    );

    /*
     * Entry 2: Kernel Data Segment
     * Same range as code segment — covers all memory.
     * Not executable — data only.
     */
    gdt_set_entry(GDT_KERNEL_DATA,
        0x00000000,
        0xFFFFFFFF,
        GDT_ACCESS_PRESENT | GDT_ACCESS_RING0 |
        GDT_ACCESS_DESCRIPTOR | GDT_ACCESS_READWRITE,
        GDT_FLAG_32BIT | GDT_FLAG_4K_GRAN
    );

    /* Load GDT into CPU */
    gdt_flush((uint32_t)&gdt_ptr);
}