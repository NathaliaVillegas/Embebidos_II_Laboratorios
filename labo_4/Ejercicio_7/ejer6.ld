/******************************************************************************
 * ejer6.ld - Linker script para Tiva TM4C1294 con soporte para interrupciones
 *****************************************************************************/

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 0x00100000
    SRAM (rwx) : ORIGIN = 0x20000000, LENGTH = 0x00040000
}

SECTIONS
{
    .text :
    {
        _text = .;
        KEEP(*(.isr_vector))
        *(.text*)
        *(.rodata*)
        _etext = .;
    } > FLASH

    /* Seccion para manejo de excepciones, alineada para evitar el overlap */
    .ARM.exidx : ALIGN(8)
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
    } > FLASH

    _ldata = ALIGN(8);

    .data : AT(_ldata) ALIGN(8)
    {
        _data = .;
        *(.data*)
        _edata = .;
    } > SRAM

    .bss :
    {
        _bss = .;
        *(.bss*)
        *(COMMON)
        _ebss = .;
    } > SRAM

    .stack :
    {
        _stack = .;
        . = . + 2048;
        _estack = .;
    } > SRAM
}
