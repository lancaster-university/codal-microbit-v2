MEMORY
{
  MBR (rx) : ORIGIN = 0x0000, LENGTH = 0x1000
  SD (rx) : ORIGIN = 0x1000, LENGTH = 0x26000
  FLASH (rx) : ORIGIN = 0x27000, LENGTH = 0x59000
  BOOTLOADER (rx) : ORIGIN = 0x72000, LENGTH = 0xC000
  SETTINGS (rx) : ORIGIN = 0x7F000, LENGTH = 0x1000
  UICR (rx) : ORIGIN = 0x10001014, LENGTH = 0x8
  RAM (rwx) : ORIGIN = 0x20002270, LENGTH = 0x1DD90
}
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(Reset_Handler)
SECTIONS
{
    . = ALIGN(4);
    .mbr :
    {
        KEEP(*(.mbr))
    } > MBR

    . = ALIGN(4);
    .softdevice :
    {
        KEEP(*(.softdevice))
    } > SD
    
    . = ALIGN(4);
    .bootloader :
    {
        KEEP(*(.bootloader))
    } > BOOTLOADER

    . = ALIGN(4);
    .uicr :
    {
        KEEP(*(.uicr))
    } > UICR

    . = ALIGN(4);
    .settings :
    {
        KEEP(*(.settings))
    } > SETTINGS
    
    .text :
    {
        KEEP(*(.isr_vector))
        KEEP(*(.Vectors))
        *(.text*)
        KEEP(*(.init))
        KEEP(*(.fini))
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)
        *(.rodata*)
        KEEP(*(.eh_frame*))
    } > FLASH
    .ARM.extab :
    {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
        . = ALIGN(4);
    } > FLASH
    __exidx_start = .;
    .ARM.exidx :
    {
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        . = ALIGN(4);
    } > FLASH
    __exidx_end = .;
    __etext = .;
    .data : AT (__etext)
    {
        __data_start__ = .;
        *(vtable)
        *(.data*)
        . = ALIGN(4);
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN (__init_array_end = .);
        . = ALIGN(4);
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);
        . = ALIGN(4);
        PROVIDE(__start_fs_data = .);
        KEEP(*(.fs_data))
        PROVIDE(__stop_fs_data = .);
        *(.jcr)
        . = ALIGN(4);
        __data_end__ = .;
    } > RAM
    __edata = .;
    .noinit :
    {
      PROVIDE(__start_noinit = .);
      KEEP(*(.noinit))
      PROVIDE(__stop_noinit = .);
    } > RAM
    .bss :
    {
        . = ALIGN(4);
        __bss_start__ = .;
        *(.bss*)
        *(COMMON)
        . = ALIGN(4);
        __bss_end__ = .;
    } > RAM
    .heap (NOLOAD):
    {
        __end__ = .;
        end = __end__;
        *(.heap*);
        ASSERT(. <= (ORIGIN(RAM) + LENGTH(RAM) - 0x800), "heap region overflowed into stack");
        . += (ORIGIN(RAM) + LENGTH(RAM) - 0x800) - .;
    } > RAM
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    PROVIDE(__stack = __StackTop);
}

