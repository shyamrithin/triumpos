# Triumph OS 🏆

A lightweight operating system built from scratch for x86-64 architecture.

## Overview

Triumph OS is a hobby operating system built with the goal of learning OS
development from the ground up — bootloader, kernel, drivers, memory management,
and eventually a graphical desktop environment.

## Goals

- [x] Boot a custom kernel on x86-64
- [ ] Memory management (GDT, paging, heap)
- [ ] Interrupt handling (IDT, keyboard, timer)
- [ ] Basic interactive shell
- [ ] Graphical framebuffer UI
- [ ] Linux compatibility layer
- [ ] Run Doom
- [ ] Boot on real hardware

## Architecture
[ Desktop UI / Window Manager ]        ← coming soon
[ Linux Compatibility Layer ]          ← coming soon
[ Kernel: Memory | Scheduler | IPC ]  ← in progress
[ Drivers: VGA | Keyboard | Disk ]    ← in progress
[ x86-64 Hardware / QEMU / VirtualBox ]

## Tech Stack

- **Language:** C + x86 Assembly (NASM)
- **Bootloader:** GRUB2 (Multiboot2)
- **Emulator:** QEMU / VirtualBox
- **Target:** x86-64 bare metal

## Building

### Prerequisites

```bash
sudo apt install build-essential nasm gcc grub-pc-bin \
                 grub-common xorriso mtools qemu-system-x86
```

### Build & Run

```bash
# Build the ISO
make

# Run in QEMU
make run

# Clean build artifacts
make clean
```

## Project Structure
triumpos/
├── boot/          # Assembly entry point + linker script
├── kernel/        # Kernel C source code
├── drivers/       # Hardware drivers
├── libc/          # Minimal C library (coming soon)
├── iso/           # Bootable ISO structure
└── build/         # Compiled output

## Milestones

### ✅ Milestone 1 — First Boot
Triumph OS boots in QEMU and prints to screen via VGA text mode driver
written entirely from scratch.

## Inspiration

Inspired by [TempleOS](https://templeos.org) by Terry Davis and
[SerenityOS](https://serenityos.org) by Andreas Kling.

## License

MIT