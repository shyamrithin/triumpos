/*
 * ============================================================
 * Triumph OS - Linker Script
 * File: boot/linker.ld
 * Description: Tells the linker how to arrange all compiled
 *              code and data into a single kernel binary.
 *              Defines memory layout of Triumph OS.
 * ============================================================
 */

ENTRY(_start)

SECTIONS {
    . = 1M;                         /* Load kernel at 1MB mark */

    .text ALIGN(4K) : {             /* Executable code section */
        *(.multiboot)               /* Multiboot header MUST be first */
        *(.text)                    /* All code goes here */
    }

    .rodata ALIGN(4K) : {           /* Read-only data (string literals etc.) */
        *(.rodata)
    }

    .data ALIGN(4K) : {             /* Initialized data */
        *(.data)
    }

    .bss ALIGN(4K) : {              /* Uninitialized data (our stack lives here) */
        *(COMMON)
        *(.bss)
    }
}