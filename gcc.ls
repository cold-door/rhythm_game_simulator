OUTPUT_ARCH(arm)

SECTIONS {
  .text 0x02000000 : { *(.text) }
  .data   : { *(.data) }
  .bss    : { *(.bss) }
  .rodata : { *(.rodata*) }
 }
