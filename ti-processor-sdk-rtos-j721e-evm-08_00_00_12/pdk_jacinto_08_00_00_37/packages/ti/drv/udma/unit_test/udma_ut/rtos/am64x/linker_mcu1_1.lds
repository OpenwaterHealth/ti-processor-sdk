/* linker options */
--fill_value=0
--stack_size=0x2000
--heap_size=0x1000

-e __VECS_ENTRY_POINT
--retain="*(.utilsCopyVecsToAtcm)"

MEMORY
{
    SBL_RSVD      (X)     : ORIGIN = 0x70000000, LENGTH = 0x80000           /* Reserved for SBL code/data */
    RESET_VECTORS (X)     : ORIGIN = 0x70080000, LENGTH = 0x100             /* Reset Vectors base address should be 64 bytes aligned  */
    MSMC3         (RWIX)  : ORIGIN = 0x70080100, LENGTH = 0x40000 - 0x1100  /* AM64x MCMS3 location */  
    VECTORS       (X)     : ORIGIN = 0x700BF000, LENGTH = 0x1000
    MSMC3_H       (RWIX)  : ORIGIN = 0x70170000, LENGTH = 0x100000          /* ~0.5MB  Reserved for SYSFW Secure Proxy */
    SYSFW_RSVD_1  (X)     : ORIGIN = 0x71C00000, LENGTH = 0x20000           /* Reserved by ROM for SYSFW */
    SYSFW_RSVD_2  (X)     : ORIGIN  =0x71E00000, LENGTH = 0x20000           /* Reserved by ROM for SYSFW */
    DDR0          (RWIX)  : ORIGIN = 0x80000000, LENGTH = 0x80000000        /* 2GB */
}

SECTIONS
{
    .vecs       : {
        __VECS_ENTRY_POINT = .;
    }                                            palign(8)   > RESET_VECTORS
    .text_boot {
        *boot.aer5f*<*boot.o*>(.text)
     }                                           palign(8)   > MSMC3
    .text:xdc_runtime_Startup_reset__I      : {} palign(8)   > MSMC3
    .text:ti_sysbios_family_arm_v7r_Cache*  : {} palign(8)   > MSMC3
    .text:ti_sysbios_family_arm_MPU*        : {} palign(8)   > MSMC3
    .utilsCopyVecsToAtcm                    : {} palign(8)   > MSMC3

    .text                                   : {} palign(8)   > DDR0
    .cinit                                  : {} palign(8)   > DDR0
    .bss                                    : {} align(8)    > DDR0
    .const                                  : {} palign(8)   > DDR0
    .data                                   : {} palign(128) > DDR0
    .sysmem                                 : {} align(8)    > DDR0
    .stack                                  : {} align(4)    > DDR0
    .data_buffer                            : {} palign(128) > DDR0

    .udma_buffer_ddr                        : {} palign(128) > DDR0
    .udma_buffer_ospi                       : {} palign(128) > DDR0
    .udma_buffer_msmc                       : {} palign(128) > MSMC3
    .udma_buffer_internal                   : {} palign(128) > MSMC3
}
