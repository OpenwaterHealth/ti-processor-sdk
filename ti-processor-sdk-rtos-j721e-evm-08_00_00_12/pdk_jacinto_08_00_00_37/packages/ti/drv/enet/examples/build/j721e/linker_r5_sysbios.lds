/* linker options */
--fill_value=0

-e __VECS_ENTRY_POINT
--retain="*(.utilsCopyVecsToAtcm)"

MEMORY
{
    R5F_TCMA_SBL_RSVD(X): ORIGIN = 0x00000000 , LENGTH = 0x100

    /* Refer the user guide for details on persistence of these sections */
    OCMC_RAM_BOARD_CFG (RWIX)   : origin=0x41C80000 length=0x2000
    OCMC_RAM_SCISERVER (RWIX)   : origin=0x41C82000 length=0x60000
    RESET_VECTORS (X)           : origin=0x41CE2000 length=0x100
    OCMC_RAM (RWIX)             : origin=0x41CE2100 length=0x1DA00
    OCMC_RAM_X509_HEADER (RWIX) : origin=0x41CFFB00 length=0x500

    R5F_TCMB0(RWIX)     : ORIGIN = 0x41010000 , LENGTH = 0x00008000
    DDR0 (RWIX)         : ORIGIN = 0x80000000 , LENGTH = 0x10000000
    DDR0_NONCACHED ( RWIX ) : ORIGIN = 0xA3000000 , LENGTH = 0x01000000
    MSRAM (RWIX)        : ORIGIN = 0x03600000 , LENGTH = 0x80000       /* 512K OCMC, used for allocation of ICSSG port queues */
    MSMC3 (RWIX)        : ORIGIN = 0x70040000 , LENGTH = 0x7B0000      /* 8MB - 320KB */
}

SECTIONS
{
    .vecs       : {
        __VECS_ENTRY_POINT = .;
    } palign(8) > RESET_VECTORS
    .text_boot {
        *boot.aer5f*<*boot.o*>(.text)
     }  palign(8)   > R5F_TCMB0
    .text:xdc_runtime_Startup_reset__I     : {} palign(8) > R5F_TCMB0
    .text:ti_sysbios_family_arm_v7r_Cache* : {} palign(8) > R5F_TCMB0
    .text:ti_sysbios_family_arm_MPU*       : {} palign(8) > R5F_TCMB0
    .utilsCopyVecsToAtcm                   : {} palign(8) > R5F_TCMB0

    /* DMA packet info (metadata), packet data and UDMA ring and desc memory */
    .bss:ENET_DMA_DESC_MEMPOOL (NOLOAD) {} ALIGN (128) > DDR0
    .bss:ENET_DMA_RING_MEMPOOL (NOLOAD) {} ALIGN (128) > DDR0
    .bss:ENET_DMA_PKT_MEMPOOL  (NOLOAD) {} ALIGN (128) > DDR0
    .bss:ENET_DMA_OBJ_MEM      (NOLOAD) {} ALIGN (128) > DDR0
    .bss:ENET_DMA_PKT_INFO_MEMPOOL (NOLOAD) {} ALIGN (128) > DDR0

    /* For NDK packet memory*/
    .bss:NDK_MMBUFFER  (NOLOAD) {} ALIGN (128) > DDR0
    .bss:NDK_PACKETMEM (NOLOAD) {} ALIGN (128) > DDR0

    .bss:ENET_ICSSG_OCMC_MEM (NOLOAD) {} ALIGN (128) > MSMC3

    /* Used in Switch configuration tool */
    .serialContext     (NOLOAD) {} ALIGN (128) > DDR0

    .text       : {} palign(8)   > DDR0
    .cinit      : {} palign(8)   > DDR0
    .bss        : {} align(8)    > DDR0
    .far        : {} align(8)    > DDR0
    .const      : {} palign(8)   > DDR0
    .data       : {} palign(128) > DDR0
    .sysmem     : {} align(8)    > DDR0
    .stack      : {} align(4)    > DDR0
    .data_buffer: {} palign(128) > DDR0
    .noncached_section: {} palign(128) > DDR0_NONCACHED
    .bss.devgroup*   : {} align(4) > DDR0
    .const.devgroup* : {} align(4) > DDR0
    .boardcfg_data   : {} align(4) > DDR0
}
