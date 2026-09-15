/* Minimal STM32F103C8T6 linker script */
/* Flash 64KB @0x08000000, RAM 20KB @0x20000000 */

/* 栈顶地址 = RAM起始地址 + RAM大小 */
_estack = 0x20005000;    /* end of RAM (20KB) */

/* 最小堆栈大小 */
_Min_Heap_Size = 0x200;   /* 512 bytes */
_Min_Stack_Size = 0x400;  /* 1KB */

MEMORY
{
  FLASH (rx)  : ORIGIN = 0x08000000, LENGTH = 64K
  RAM   (rwx) : ORIGIN = 0x20000000, LENGTH = 20K
}

ENTRY(Reset_Handler)

SECTIONS
{
  .isr_vector :
  {
    KEEP(*(.isr_vector))
  } > FLASH

  .text :
  {
    *(.text*)
    *(.rodata*)
    *(.glue_7t) *(.glue_7)
    *(.init) *(.fini)
    KEEP(*(.init))
    KEEP(*(.fini))
    /* Constructors / destructors */
    . = ALIGN(4);
    KEEP(*crtbegin.o(.ctors))
    KEEP(*(EXCLUDE_FILE(*crtend.o) .ctors))
    KEEP(*(SORT(.ctors.*)))
    KEEP(*(.ctors))
    KEEP(*crtbegin.o(.dtors))
    KEEP(*(EXCLUDE_FILE(*crtend.o) .dtors))
    KEEP(*(SORT(.dtors.*)))
    KEEP(*(.dtors))
    . = ALIGN(4);
    _etext = .;
  } > FLASH

  .ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) } > FLASH
  .ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) } > FLASH

  .preinit_array :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP(*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } > FLASH

  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP(*(SORT(.init_array.*)))
    KEEP(*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } > FLASH

  .fini_array :
  {
    PROVIDE_HIDDEN (__fini_array_start = .);
    KEEP(*(SORT(.fini_array.*)))
    KEEP(*(.fini_array*))
    PROVIDE_HIDDEN (__fini_array_end = .);
  } > FLASH

  . = ALIGN(4);
  _sidata = .;

  .data : AT (LOADADDR(.preinit_array) + SIZEOF(.preinit_array) + SIZEOF(.init_array) + SIZEOF(.fini_array))
  {
    _sdata = .;
    *(.data*)
    . = ALIGN(4);
    _edata = .;
  } > RAM

  .bss (NOLOAD) :
  {
    _sbss = .;
    __bss_start__ = _sbss;
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    _ebss = .;
    __bss_end__ = _ebss;
  } > RAM

  ._user_heap_stack (NOLOAD):
  {
    . = ALIGN(8);
    PROVIDE( _end = . );
    PROVIDE( end = . );
    KEEP(*(.heap*))
    KEEP(*(.stack*))
  } > RAM

  /* Stabs debugging sections. */
  .stab          0 : { *(.stab) }
  .stabstr       0 : { *(.stabstr) }
  .comment       0 : { *(.comment) }
  .debug         0 : { *(.debug) }
  .line          0 : { *(.line) }
  .debug_srcinfo 0 : { *(.debug_srcinfo) }
  .debug_sfnames 0 : { *(.debug_sfnames) }
  .debug_aranges 0 : { *(.debug_aranges) }
  .debug_pubnames 0 : { *(.debug_pubnames) }
  .debug_info    0 : { *(.debug_info) }
  .debug_abbrev  0 : { *(.debug_abbrev) }
  .debug_line    0 : { *(.debug_line) }
  .debug_frame   0 : { *(.debug_frame) }
  .debug_str     0 : { *(.debug_str) }
  .debug_loc     0 : { *(.debug_loc) }
  .debug_macinfo 0 : { *(.debug_macinfo) }
}
