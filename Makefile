# ============================================================
# Triumph OS - Makefile
# File: Makefile
# Description: Builds the Triumph OS kernel, assembles all
#              source files, links them into a binary, and
#              packages everything into a bootable ISO.
# ============================================================

# ── Compiler & Tools ────────────────────────────────────────
CC      = gcc
ASM     = nasm
LD      = ld
GRUB    = grub-mkrescue

# ── Flags ───────────────────────────────────────────────────
CFLAGS  = -m32 \
           -ffreestanding \
           -fno-stack-protector \
           -fno-builtin \
           -nostdlib \
           -nostdinc \
		   -isystem /usr/lib/gcc/x86_64-linux-gnu/11/include \
           -Wall \
           -Wextra \
           -Ikernel

ASMFLAGS = -f elf32

LDFLAGS  = -m elf_i386 \
            -T boot/linker.ld \
            -nostdlib

# ── Output Names ────────────────────────────────────────────
KERNEL_BIN  = iso/boot/triumpos.bin
ISO_FILE    = build/triumpos.iso

# ── Source Files ────────────────────────────────────────────
ASM_SOURCES = boot/entry.asm \
              boot/gdt_flush.asm
C_SOURCES   = kernel/kernel.c \
              kernel/gdt.c

# ── Object Files ────────────────────────────────────────────
ASM_OBJECTS = $(ASM_SOURCES:.asm=.o)
C_OBJECTS   = $(C_SOURCES:.c=.o)
OBJECTS     = $(ASM_OBJECTS) $(C_OBJECTS)

# ── Default Target ───────────────────────────────────────────
.PHONY: all clean run

all: $(ISO_FILE)

# ── Step 1: Assemble .asm → .o ───────────────────────────────
%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

# ── Step 2: Compile .c → .o ──────────────────────────────────
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ── Step 3: Link all .o → kernel binary ──────────────────────
$(KERNEL_BIN): $(OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# ── Step 4: Package into bootable ISO ────────────────────────
$(ISO_FILE): $(KERNEL_BIN)
	$(GRUB) -o $@ iso/

# ── Run in QEMU ──────────────────────────────────────────────
run: $(ISO_FILE)
	qemu-system-x86_64 -cdrom $(ISO_FILE) -m 512M

# ── Clean build artifacts ─────────────────────────────────────
clean:
	rm -f $(OBJECTS) $(KERNEL_BIN) $(ISO_FILE)