/* Linker Settings */
--retain="*(.bootCode)"
--retain="*(.startupCode)"
--retain="*(.startupData)"
--retain="*(.intvecs)"
--retain="*(.intc_text)"
--retain="*(.rstvectors)"
--retain="*(.irqStack)"
--retain="*(.fiqStack)"
--retain="*(.abortStack)"
--retain="*(.undStack)"
--retain="*(.svcStack)"
--fill_value=0
--stack_size=0x2000
--heap_size=0x1000
--entry_point=_resetvectors     /* Default C RTS boot.asm   */

-stack  0x2000                              /* SOFTWARE STACK SIZE           */
-heap   0x2000                              /* HEAP AREA SIZE                */

/* Stack Sizes for various modes */
__IRQ_STACK_SIZE = 0x1000;
__FIQ_STACK_SIZE = 0x1000;
__ABORT_STACK_SIZE = 0x1000;
__UND_STACK_SIZE = 0x1000;
__SVC_STACK_SIZE = 0x1000;

/* Memory Map */
MEMORY
{
     
    /* Reserved for SBL code/data */
    SBL_RSVD (X)            : origin=0x70000000 length=0x80000
    
    /*  Reset Vectors base address(RESET_VECTORS) should be 64 bytes aligned  */
    RESET_VECTORS (X)       : origin=0x70080000 length=0x100
    /* am64x MCMS3 locations */
    MSMC3   (RWIX)          : origin=0x70080100 length=0x40000 - 0x1100

    VECTORS (X)             : origin=0x700BF000 length=0x1000
    /* Reserved for SYSFW Secure Proxy */
    MSMC3_H (RWIX)          : origin=0x700C0000 length=0x100000   

    /* Reserved by ROM for SYSFW */
    SYSFW_RSVD_1 (X)      : origin=0x71C00000 length=0x20000 
    SYSFW_RSVD_2 (X)      : origin=0x71E00000 length=0x20000 

    DDR0    (RWIX)          : origin=0x80000000 length=0x80000000  /* 2GB */
}

/* Section Configuration */
SECTIONS
{
    /* 'intvecs' and 'intc_text' sections shall be placed within */
    /* a range of +\- 16 MB */
    .intvecs       : {} palign(8)      > VECTORS
    .intc_text     : {} palign(8)      > VECTORS
    .rstvectors    : {} palign(8)      > RESET_VECTORS 
    .bootCode      : {} palign(8)      > MSMC3
    .startupCode   : {} palign(8)      > MSMC3
    .startupData   : {} palign(8)      > MSMC3, type = NOINIT
    .text          : {} palign(8)      > DDR0
    .const         : {} palign(8)      > DDR0
    .cinit         : {} palign(8)      > DDR0
    .pinit         : {} palign(8)      > DDR0
    .bss           : {} align(4)       > DDR0
    .far           : {} align(4)       > DDR0
    .data          : {} palign(128)    > DDR0
    .boardcfg_data : {} palign(128)    > DDR0
    .sysmem        : {}                > DDR0
    .data_buffer   : {} palign(128)    > DDR0

    .udma_buffer_ddr                        : {} palign(128) > DDR0
    .udma_buffer_ospi                       : {} palign(128) > DDR0
    .udma_buffer_msmc                       : {} palign(128) > MSMC3
    .udma_buffer_internal                   : {} palign(128) > MSMC3

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) {} ALIGN (8) > DDR0

    .stack      : {} align(4)       > DDR0  (HIGH)
    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > DDR0  (HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)
    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > DDR0  (HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)
    .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > DDR0  (HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)
    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > DDR0  (HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)
    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > DDR0  (HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)
}
