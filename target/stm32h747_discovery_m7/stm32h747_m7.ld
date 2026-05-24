MEMORY
{
    FLASH (rx)      : ORIGIN = 0x08000000, LENGTH = 1024K
    SRAM (xrw)      : ORIGIN = 0x20000000, LENGTH = 128K
}

PROVIDE(hardware_init_hook = 0);
PROVIDE(software_init_hook = 0);
PROVIDE(software_term_hook = 0);


/*------------------ここから STM32Cube から -------------------*/
/* Highest address of the user mode stack */
_estack = 0x20000000+0x20000;    /* end of RAM */
/* Generate a link error if heap and stack don't fit into RAM */
_Min_Heap_Size = 0x200 ;      /* required amount of heap  */
_Min_Stack_Size = 0x400 ; /* required amount of stack */
/*------------------ここまで STM32Cube から -------------------*/

ENTRY(_kernel_start)

SECTIONS
{
	.text :
	{
	    . = ALIGN(4);
		__text = .;
		*(.vector)
		*(.text)
		*(.text.*)
		KEEP(*(.empty.*))
	    . = ALIGN(4);
	} > FLASH
	_etext = .;
	PROVIDE(etext = .);

	.rodata :
	{
	    . = ALIGN(4);
		*(.rodata)
		*(.rodata.*)
	    . = ALIGN(4);
	} > FLASH

	.ARM.extab   : { *(.ARM.extab* .gnu.linkonce.armextab.*) }
	__exidx_start = .;
	.ARM.exidx   : { *(.ARM.exidx* .gnu.linkonce.armexidx.*) }
	__exidx_end = .;

	. = ALIGN(4);
	__idata_start = .;
	.data : AT (__idata_start)
	{
	    . = ALIGN(4);
		__data_start = .;
		*(.data)
		*(.data.*)
	    . = ALIGN(4);
	} > SRAM
	__idata_end = __idata_start + SIZEOF(.data);
	_edata = .;
	PROVIDE(edata = .);

	. = ALIGN(4);

	.bss :
	{
	    . = ALIGN(4);
		__bss_start  = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
	} > SRAM
	. = ALIGN(4);
	_end = .;
	__bss_end = .;
	PROVIDE(end = .);
}
