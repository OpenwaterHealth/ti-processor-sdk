/* linker options */
--fill_value=0
--stack_size=0x2000
--heap_size=0x1000

-e __VECS_ENTRY_POINT
--retain="*(.utilsCopyVecsToAtcm)"

MEMORY
{
    R5F_TCMA_SBL_RSVD(X): ORIGIN = 0x00000000, LENGTH = 0x100
    R5F_TCMB0   (RWIX)  : ORIGIN = 0x41010000, LENGTH = 0x00008000  /*  32 KB */
    DDR0        (RWIX)  : ORIGIN = 0x80000000, LENGTH = 0x08000000  /* 128 MB per core */
    /* Refer the user guide for details on persistence of these sections */
    OCMC_RAM_BOARD_CFG (RWIX)   : origin=0x41c80000 length=0x2000
    VECTORS (X)                 : origin=0x41C82000 length=0x1000
    RESET_VECTORS (X)           : origin=0x41C83000 length=0x100
    OCMC_RAM (RWIX)             : origin=0x41C83100 length=0x8000
    OCMC_RAM_X509_HEADER (RWIX) : origin=0x41cffb00 length=0x500
    MSMC3       (RWIX)  : ORIGIN = 0x70040000, LENGTH = 0x000B0000  /* 1MB - 320 KB  */
}

SECTIONS
{
    .vecs       : {
        __VECS_ENTRY_POINT = .;
    }                                            palign(8)   > RESET_VECTORS
    .text_boot {
        *boot.aer5f*<*boot.o*>(.text)
     }                                           palign(8)   > R5F_TCMB0
    .text:xdc_runtime_Startup_reset__I      : {} palign(8)   > R5F_TCMB0
    .text:ti_sysbios_family_arm_v7r_Cache*  : {} palign(8)   > R5F_TCMB0
    .text:ti_sysbios_family_arm_MPU*        : {} palign(8)   > R5F_TCMB0
    .utilsCopyVecsToAtcm                    : {} palign(8)   > R5F_TCMB0

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
    .udma_buffer_internal                   : {} palign(128) > OCMC_RAM
}
