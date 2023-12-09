/* Linker Settings                                                            */
/* Standard linker options                                                    */
--retain="*(.intvecs)"
--retain="*(.intc_text)"
--retain="*(.rstvectors)"
--fill_value=0
--stack_size=0x2000
--heap_size=0x1000
/* Retain entry point */
-e __VECS_ENTRY_POINT

-stack  0x2000                              /* SOFTWARE STACK SIZE           */
-heap   0x2000                              /* HEAP AREA SIZE                */

MEMORY
{
    VECTORS (X)                 : origin=0x41C7F000 length=0x1000
    RESET_VECTORS (X)           : origin=0x41C00000 length=0x100                /*  Reset Vectors base address(RESET_VECTORS) should be 64 bytes aligned  */
    MCU0_R5F_TCMA_SBL_RSVD (X)  : origin=0x0        length=0x100                /* MCU0_R5F_0 local view */
    MCU0_R5F_TCMA (X)           : origin=0x100      length=0x8000 - 0x100
    MCU0_R5F_TCMB0 (RWIX)       : origin=0x41010000 length=0x8000

    /* MCU0_R5F_1 SoC view */
    MCU0_R5F1_ATCM (RWIX)       : origin=0x41400000 length=0x8000
    MCU0_R5F1_BTCM (RWIX)       : origin=0x41410000 length=0x8000

    /* Fully avaialble for apps. Used by SBL to load SYSFW */
    OCMRAM_LOW  (RWIX)          : origin=0x41C00100 length=0x40600 - 0x100      /* ~257KB */

    /* Mem mapped OSPI memory */
    OSPI_MEM    (RWIX)          : origin=0x50000000 length=0x8000000            /* 128MB */

    /* MCU0 memory used for SBL. Avaiable after boot for app starts for dynamic use */
    SBL_RESERVED    (RWIX)      : origin=0x41C40600 length=0x60000 - 0x40600    /* ~126KB */

    /* MCU0 share locations */
    OCMRAM  (RWIX)              : origin=0x41C60000 length=0x20000 - 0x1000     /* ~124KB */

    /* AM65XX M4 locations */
    MSMC3   (RWIX)              : origin=0x70000000 length=0xF0000              /* 1MB - 64K */
    MSMC3_DMSC (RWIX)           : origin=0x700F0000 length=0x10000              /* Reserved for DMSC - 64K */
    MSMC3_H (RWIX)              : origin=0x70100000 length=0xF2000              /* 1MB -56K */
    MSMC3_NOCACHE (RWIX)        : origin=0x701F2000 length=0xE000               /* 56K */
    DDR0    (RWIX)              : origin=0x80000000 length=0x80000000           /* 2GB */
}

SECTIONS
{
    /* Place sysbios entry point - starts */
    .vecs : { *(.vecs) } palign(8) > VECTORS
    .vecs : { __VECS_ENTRY_POINT = .; } > VECTORS

    xdc.meta (COPY): { *(xdc.meta) } > OCMRAM
    .init_text  : {
                     boot.*(.text)
                     *(.text:ti_sysbios_family_arm_MPU_*)
                     *(.text:ti_sysbios_family_arm_v7r_Cache_*)
                  }  palign(8) > OCMRAM

    .text:xdc_runtime_Startup_reset__I: {} palign(8) > OCMRAM

    .bootCode       : {} palign(8) > OCMRAM
    .startupCode    : {} palign(8) > OCMRAM
    .startupData    : {} palign(8) > OCMRAM, type = NOINIT
    .utilsCopyVecsToAtcm : {} palign(8) > OCMRAM

    /* Place sysbios entry point - ends */

    .text                   : {} palign(8)      > MSMC3
    .const                  : {} palign(8)      > MSMC3
    .cinit                  : {} palign(8)      > MSMC3
    .pinit                  : {} palign(8)      > MSMC3
    .bss                    : {} align(4)       > MSMC3
    .data                   : {} palign(128)    > MSMC3
    .boardcfg_data          : {} palign(128)    > MSMC3
    .udma_buffer_msmc       : {} palign(128)    > MSMC3
    .udma_buffer_ddr        : {} palign(128)    > DDR0
    .udma_buffer_internal   : {} palign(128)    > OCMRAM_LOW
    .udma_buffer_ospi       : {} palign(128)    > DDR0
    .sysmem                 : {}                > MSMC3
    .stack                  : {} align(4)       > MSMC3 (HIGH)
}
