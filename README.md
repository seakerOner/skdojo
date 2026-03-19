# Seaker's Dojo

**Seaker's Dojo (skdojo)** is a bare-metal computing environment 
designed for direct interaction with the machine

It is not a tradicional operating system. 
Maybe it is not an operating system at all in the modern sense, 
where the responsabilities and use cases are uncountable.

skdojo is a **dojo for computation**; a place to create, run, and explore programs from within the system itself.

## status

- 2 stage bootloader for x86 systems
    - Boot from BIOS
    - Loads disk sections with BIOS extensions
    - RAM detection
    - A20 line
    - GDT
    - IDT
    - Protected Mode 
    - Long mode (64-bit)
- Kernel structure in C
    - VGA text ouput
    - Keyboard Input (IRQ-driven)

## In Progress

- Further develop Bootloader and Kernel
- Block-based system implementation
- Dojo environment (REPL / interactive programming model)

# Notes

- Writen in:
    - Assembly (bootloader)
    - C (kernel)
- Boot process:
    - BIOS -> Stage 1 -> Stage 2 -> Kernel
- VGA text mode used for output
- Interrupts used for input handling
