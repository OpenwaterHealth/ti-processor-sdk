__STACK_SIZE = 0x20000;
__TI_STACK_SIZE = __STACK_SIZE;

MEMORY
{
    /* am64x MCMS3 locations                                                  */

    /* Reserved for SBL code/data */
    SBL_RSVD	(X)            : ORIGIN = 0x70000000, LENGTH = 0x80000

    /* am64x Reserved Memory for ARM Trusted Firmware                        */
    MSMC3_ARM_STARTUP	(RWIX)   : ORIGIN = 0x000070080000, LENGTH = 0x800
    MSMC_MPU2	(RWX)	    : ORIGIN = 0x000070080800, LENGTH = 0x800
    MSMC_MPU1	(RWX)	    : ORIGIN = 0x000070081000, LENGTH = 0x40000-0x1000

    /* Reserved for SYSFW Secure Proxy */
    MSMC3_H	(RWIX)          : ORIGIN = 0x700C0000, LENGTH = 0x100000   

    /* Reserved by ROM for SYSFW */
    SYSFW_RSVD_1	(X)      : ORIGIN = 0x71C00000, LENGTH = 0x20000 
    SYSFW_RSVD_2	(X)      : ORIGIN = 0x71E00000, LENGTH = 0x20000 

    DDR_MPU1	(RWX)	    : ORIGIN =  0x80000000, LENGTH = 0x08000000

}
REGION_ALIAS("REGION_TEXT_EL3", DDR_MPU1);
REGION_ALIAS("BOOTVECTOR_EL3", DDR_MPU1);
REGION_ALIAS("BOOTVECTOR", DDR_MPU1);
REGION_ALIAS("REGION_TEXT", DDR_MPU1);
REGION_ALIAS("REGION_BSS", DDR_MPU1);
REGION_ALIAS("REGION_DATA", DDR_MPU1);
REGION_ALIAS("REGION_STACK", DDR_MPU1);
REGION_ALIAS("REGION_HEAP", DDR_MPU1);
REGION_ALIAS("REGION_ARM_EXIDX", DDR_MPU1);
REGION_ALIAS("REGION_ARM_EXTAB", DDR_MPU1);
REGION_ALIAS("REGION_TEXT_STARTUP", DDR_MPU1);
REGION_ALIAS("REGION_DATA_BUFFER", DDR_MPU1);
REGION_ALIAS("IPC_DATA_BUFFER_1", DDR_MPU1);
REGION_ALIAS("REGION_UDMA_BUFFER_MSMC", MSMC_MPU1);
REGION_ALIAS("REGION_UDMA_BUFFER_DDR", DDR_MPU1);
REGION_ALIAS("REGION_UDMA_BUFFER_OSPI", DDR_MPU1);
REGION_ALIAS("REGION_UDMA_BUFFER_INTERNAL", DDR_MPU1);

SECTIONS {

    .vecs : {
        *(.vecs)
    } > BOOTVECTOR AT> DDR_MPU1

    .vectors : {
        *(.vectors)
    } > BOOTVECTOR_EL3 AT> DDR_MPU1
	.text.el3 : {
	    *(.text.el3)
		 /* Ensure 8-byte alignment for descriptors and ensure inclusion */
        . = ALIGN(8);
        __RT_SVC_DESCS_START__ = .;
        KEEP(*(rt_svc_descs))
        __RT_SVC_DESCS_END__ = .;
    } > REGION_TEXT_EL3 AT> DDR_MPU1

    .text.csl_a53_startup : {
        *(.text.csl_a53_startup)
		*(.Entry)
    } > REGION_TEXT_STARTUP AT> REGION_TEXT_STARTUP

    .text : {
        CREATE_OBJECT_SYMBOLS
        *(.text)
        *(.text.*)
        . = ALIGN(0x8);
        KEEP (*(.ctors))
        . = ALIGN(0x4);
        KEEP (*(.dtors))
        . = ALIGN(0x8);
        __init_array_start = .;
        KEEP (*(.init_array*))
        __init_array_end = .;
        *(.init)
        *(.fini*)
    } > REGION_TEXT AT> REGION_TEXT

    PROVIDE (__etext = .);
    PROVIDE (_etext = .);
    PROVIDE (etext = .);

    .rodata : {
        *(.rodata)
        *(.rodata*)
    } > REGION_TEXT AT> REGION_TEXT

    .data_buffer : ALIGN (8) {
        __data_buffer_load__ = LOADADDR (.data_buffer);
        __data_buffer_start__ = .;
        *(.data_buffer)
        *(.data_buffer*)
        . = ALIGN (8);
        __data_buffer_end__ = .;
    } > REGION_DATA_BUFFER AT> REGION_DATA_BUFFER

    .data : ALIGN (8) {
        __data_load__ = LOADADDR (.data);
        __data_start__ = .;
        *(.data)
        *(.data*)
        . = ALIGN (8);
        __data_end__ = .;
    } > REGION_DATA AT> REGION_TEXT

    .ARM.exidx : {
        __exidx_start = .;
        *(.ARM.exidx* .gnu.linkonce.armexidx.*)
        __exidx_end = .;
    } > REGION_ARM_EXIDX AT> REGION_ARM_EXIDX

    .ARM.extab : {
        *(.ARM.extab* .gnu.linkonce.armextab.*)
    } > REGION_ARM_EXTAB AT> REGION_ARM_EXTAB

    .bss:extMemCache:ramdisk : {
    } > DDR_MPU1  /* MSMC_MPU1 */

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) : ALIGN (32) {
    } > DDR_MPU1

    .bss:frameBuffer (NOLOAD) : ALIGN (32) {
    } > DDR_MPU1

    .udma_buffer_msmc (NOLOAD) : ALIGN (128) {
    } > REGION_UDMA_BUFFER_MSMC

    .udma_buffer_ddr (NOLOAD) : ALIGN (128) {
    } > REGION_UDMA_BUFFER_DDR

    .udma_buffer_internal (NOLOAD) : ALIGN (128) {
    } > REGION_UDMA_BUFFER_INTERNAL

    .udma_buffer_ospi (NOLOAD) : ALIGN (128) {
    } > REGION_UDMA_BUFFER_OSPI

    ipc_data_buffer (NOLOAD) : ALIGN (32) {
    } > IPC_DATA_BUFFER_1

    /* For NDK packet memory, we need to map this sections before .bss*/
    .bss:NDK_PACKETMEM (NOLOAD) : ALIGN (128) {} > DDR_MPU1
    .bss:NDK_MMBUFFER  (NOLOAD) : ALIGN (128) {} > DDR_MPU1

    .bss : {
        __bss_start__ = .;
        *(.shbss)
        *(.bss)
        *(.bss.*)
        . = ALIGN (8);
        __bss_end__ = .;
        . = ALIGN (8);
        *(COMMON)
    } > REGION_BSS AT> REGION_BSS

    .heap : {
        __heap_start__ = .;
        end = __heap_start__;
        _end = end;
        __end = end;
        KEEP(*(.heap))
        __heap_end__ = .;
        __HeapLimit = __heap_end__;
    } > REGION_HEAP AT> REGION_HEAP

    .stack (NOLOAD) : ALIGN(16) {
        _stack = .;
        __stack = .;
        KEEP(*(.stack))
    } > REGION_STACK AT> REGION_STACK

	__TI_STACK_BASE = __stack;

    /* Stabs debugging sections.  */
    .stab          0 : { *(.stab) }
    .stabstr       0 : { *(.stabstr) }
    .stab.excl     0 : { *(.stab.excl) }
    .stab.exclstr  0 : { *(.stab.exclstr) }
    .stab.index    0 : { *(.stab.index) }
    .stab.indexstr 0 : { *(.stab.indexstr) }
    .comment       0 : { *(.comment) }
    /*
     * DWARF debug sections.
     * Symbols in the DWARF debugging sections are relative to the beginning
     * of the section so we begin them at 0.
     */
    /* DWARF 1 */
    .debug         0 : { *(.debug) }
    .line          0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line .debug_line.* .debug_line_end ) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }
    /* DWARF 3 */
    .debug_pubtypes 0 : { *(.debug_pubtypes) }
    .debug_ranges   0 : { *(.debug_ranges) }
    /* DWARF Extension.  */
    .debug_macro    0 : { *(.debug_macro) }
    .note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
    /DISCARD/ : { *(.note.GNU-stack) *(.gnu_debuglink) *(.gnu.lto_*) }
}
