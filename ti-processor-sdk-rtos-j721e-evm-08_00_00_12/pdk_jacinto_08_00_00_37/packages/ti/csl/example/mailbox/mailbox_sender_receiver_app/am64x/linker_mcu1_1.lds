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
    RESET_VECTORS (X)       : origin=0x0 length=0x100

    /* am64x MCMS3 locations */
    MSMC3_MCU1_0   (RWIX)          : origin=0x70080000 length=0x1F000
    MSMC3_MCU1_1   (RWIX)          : origin=0x700A0000 length=0x1F000

    VECTORS_MCU1_0 (X)             : origin=0x7009F000 length=0x1000
    VECTORS_MCU1_1 (X)             : origin=0x700BF000 length=0x1000

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
    .intvecs       : {} palign(8)      > VECTORS_MCU1_1
    .intc_text     : {} palign(8)      > VECTORS_MCU1_1
    .rstvectors    : {} palign(8)      > RESET_VECTORS
    .bootCode      : {} palign(8)      > MSMC3_MCU1_1
    .startupCode   : {} palign(8)      > MSMC3_MCU1_1
    .startupData   : {} palign(8)      > MSMC3_MCU1_1, type = NOINIT
    .text          : {} palign(8)      > MSMC3_MCU1_1
    .const         : {} palign(8)      > MSMC3_MCU1_1
    .cinit         : {} palign(8)      > MSMC3_MCU1_1
    .pinit         : {} palign(8)      > MSMC3_MCU1_1
    .bss           : {} align(4)       > MSMC3_MCU1_1
    .far           : {} align(4)       > MSMC3_MCU1_1
    .data          : {} palign(128)    > MSMC3_MCU1_1
    .boardcfg_data : {} palign(128)    > MSMC3_MCU1_1
    .sysmem        : {}                > MSMC3_MCU1_1
    .data_buffer   : {} palign(128)    > MSMC3_MCU1_1

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) {} ALIGN (8) > MSMC3_MCU1_1

    .stack      : {} align(4)       > MSMC3_MCU1_1
    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > MSMC3_MCU1_1
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)
    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > MSMC3_MCU1_1
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)
    .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > MSMC3_MCU1_1
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)
    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > MSMC3_MCU1_1
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)
    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > MSMC3_MCU1_1
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)
}
