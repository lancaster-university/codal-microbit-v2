MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000, LENGTH = 0x7F000
  STORAGE (rx) : ORIGIN = 0x7F000, LENGTH = 0x1000
  UICR (rx) : ORIGIN = 0x10001014, LENGTH = 0x8
  NOINIT (rwx) : ORIGIN = 0x2000000, LENGTH = 0x4
  RAM (rwx) : ORIGIN = 0x20000004, LENGTH = 0x20020000 - 0x20000004
}
OUTPUT_FORMAT ("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
ENTRY(Reset_Handler)
SECTIONS
{
    . = ALIGN(4);
    .uicr :
    {
        KEEP(*(.uicr))
    } > UICR
  
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
    
    /* for NRF_LOG_XXX */
    .log_const_data :
    {
      PROVIDE(__start_log_const_data = .);
      KEEP(*(SORT(.log_const_data*)))
      PROVIDE(__stop_log_const_data = .);
    } > FLASH
    
    .sdh_ble_observers :
    {
      PROVIDE(__start_sdh_ble_observers = .);
      KEEP(*(SORT(.sdh_ble_observers*)))
      PROVIDE(__stop_sdh_ble_observers = .);
    } > FLASH
    .sdh_soc_observers :
    {
      PROVIDE(__start_sdh_soc_observers = .);
      KEEP(*(SORT(.sdh_soc_observers*)))
      PROVIDE(__stop_sdh_soc_observers = .);
    } > FLASH
    .pwr_mgmt_data :
    {
      PROVIDE(__start_pwr_mgmt_data = .);
      KEEP(*(SORT(.pwr_mgmt_data*)))
      PROVIDE(__stop_pwr_mgmt_data = .);
    } > FLASH
    .sdh_state_observers :
    {
      PROVIDE(__start_sdh_state_observers = .);
      KEEP(*(SORT(.sdh_state_observers*)))
      PROVIDE(__stop_sdh_state_observers = .);
    } > FLASH
    .sdh_stack_observers :
    {
      PROVIDE(__start_sdh_stack_observers = .);
      KEEP(*(SORT(.sdh_stack_observers*)))
      PROVIDE(__stop_sdh_stack_observers = .);
    } > FLASH
    .sdh_req_observers :
    {
      PROVIDE(__start_sdh_req_observers = .);
      KEEP(*(SORT(.sdh_req_observers*)))
      PROVIDE(__stop_sdh_req_observers = .);
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
        
        /* for NRF_LOG_XXX */
        . = ALIGN(4);
        PROVIDE(__start_log_dynamic_data = .);
        KEEP(*(SORT(.log_dynamic_data*)))
        PROVIDE(__stop_log_dynamic_data = .);

        /* for NRF_LOG_XXX */
        . = ALIGN(4);
        PROVIDE(__start_log_filter_data = .);
        KEEP(*(SORT(.log_filter_data*)))
        PROVIDE(__stop_log_filter_data = .);
        
        . = ALIGN(4);
        PROVIDE(__start_fs_data = .);
        KEEP(*(.fs_data))
        PROVIDE(__stop_fs_data = .);
        *(.jcr)
        . = ALIGN(4);
        __data_end__ = .;
    } > RAM
    __edata = .;
    .noinit (NOLOAD):
    {
      PROVIDE(__start_noinit = .);
      KEEP(*(.noinit))
      PROVIDE(__stop_noinit = .);
    } > NOINIT
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

