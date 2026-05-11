/*
 * ============================================================
 * Triumph OS - Kernel Main
 * File: kernel/kernel.c
 * Description: The C entry point of Triumph OS. Initializes
 *              the VGA text buffer and prints to the screen.
 *              This is the first C code Triumph OS executes.
 * ============================================================
 */

#include <stdint.h>
#include <stddef.h>

/* ── VGA Constants ─────────────────────────────────────────── */
#define VGA_ADDRESS     0xB8000     /* Physical address of VGA text buffer */
#define VGA_WIDTH       80          /* Standard VGA text mode columns      */
#define VGA_HEIGHT      25          /* Standard VGA text mode rows         */

/* ── VGA Colors ────────────────────────────────────────────── */
typedef enum {
    COLOR_BLACK         = 0,
    COLOR_BLUE          = 1,
    COLOR_GREEN         = 2,
    COLOR_CYAN          = 3,
    COLOR_RED           = 4,
    COLOR_MAGENTA       = 5,
    COLOR_BROWN         = 6,
    COLOR_LIGHT_GREY    = 7,
    COLOR_DARK_GREY     = 8,
    COLOR_LIGHT_BLUE    = 9,
    COLOR_LIGHT_GREEN   = 10,
    COLOR_LIGHT_CYAN    = 11,
    COLOR_LIGHT_RED     = 12,
    COLOR_LIGHT_MAGENTA = 13,
    COLOR_YELLOW        = 14,
    COLOR_WHITE         = 15,
} vga_color;

/* ── VGA State ──────────────────────────────────────────────── */
static uint16_t* vga_buffer;       /* Pointer to VGA memory               */
static size_t    cursor_row;       /* Current cursor row                   */
static size_t    cursor_col;       /* Current cursor column                */
static uint8_t   current_color;    /* Current text color attribute         */

/* ── Helper: Pack fg + bg color into one byte ──────────────── */
static inline uint8_t vga_make_color(vga_color fg, vga_color bg) {
    return fg | (bg << 4);
}

/* ── Helper: Pack character + color into a VGA entry (2 bytes) */
static inline uint16_t vga_make_entry(char c, uint8_t color) {
    return (uint16_t)c | ((uint16_t)color << 8);
}

/* ── Initialize VGA ─────────────────────────────────────────── */
void vga_init(void) {
    vga_buffer    = (uint16_t*)VGA_ADDRESS;
    cursor_row    = 0;
    cursor_col    = 0;
    current_color = vga_make_color(COLOR_LIGHT_GREEN, COLOR_BLACK);

    /* Clear the screen */
    for (size_t row = 0; row < VGA_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[row * VGA_WIDTH + col] = vga_make_entry(' ', current_color);
        }
    }
}

/* ── Scroll screen up one line ──────────────────────────────── */
static void vga_scroll(void) {
    /* Move every row up by one */
    for (size_t row = 1; row < VGA_HEIGHT; row++) {
        for (size_t col = 0; col < VGA_WIDTH; col++) {
            vga_buffer[(row - 1) * VGA_WIDTH + col] =
                vga_buffer[row * VGA_WIDTH + col];
        }
    }
    /* Clear the last row */
    for (size_t col = 0; col < VGA_WIDTH; col++) {
        vga_buffer[(VGA_HEIGHT - 1) * VGA_WIDTH + col] =
            vga_make_entry(' ', current_color);
    }
    cursor_row = VGA_HEIGHT - 1;
}

/* ── Put a single character on screen ──────────────────────── */
void vga_putchar(char c) {
    if (c == '\n') {
        cursor_col = 0;
        cursor_row++;
    } else {
        vga_buffer[cursor_row * VGA_WIDTH + cursor_col] =
            vga_make_entry(c, current_color);
        cursor_col++;
    }

    /* Wrap to next line if we hit the edge */
    if (cursor_col >= VGA_WIDTH) {
        cursor_col = 0;
        cursor_row++;
    }

    /* Scroll if we hit the bottom */
    if (cursor_row >= VGA_HEIGHT) {
        vga_scroll();
    }
}

/* ── Print a string ─────────────────────────────────────────── */
void vga_print(const char* str) {
    for (size_t i = 0; str[i] != '\0'; i++) {
        vga_putchar(str[i]);
    }
}

/* ── Print a string in a specific color ─────────────────────── */
void vga_print_color(const char* str, vga_color fg, vga_color bg) {
    uint8_t saved = current_color;
    current_color = vga_make_color(fg, bg);
    vga_print(str);
    current_color = saved;
}

/* ── Kernel Entry Point ─────────────────────────────────────── */
void kernel_main(void) {
    vga_init();

    vga_print_color("Triumph OS\n", COLOR_YELLOW, COLOR_BLACK);
    vga_print_color("===========\n", COLOR_DARK_GREY, COLOR_BLACK);
    vga_print("Kernel booted successfully.\n");
    vga_print("Welcome to Triumph OS!\n");

    /* Kernel halts here for now - more to come! */
    while (1) {}
}