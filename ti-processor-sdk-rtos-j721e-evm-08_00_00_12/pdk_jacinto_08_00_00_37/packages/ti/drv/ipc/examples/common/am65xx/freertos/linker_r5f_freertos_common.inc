/*=========================*/
/*     Linker Settings     */
/*=========================*/

--retain="*(.bootCode)"
--retain="*(.startupCode)"
--retain="*(.startupData)"
--retain="*(.irqStack)"
--retain="*(.fiqStack)"
--retain="*(.abortStack)"
--retain="*(.undStack)"
--retain="*(.svcStack)"

--fill_value=0
--stack_size=0x4000
--heap_size=0x8000
--entry_point=_freertosresetvectors

-stack  0x4000  /* SOFTWARE STACK SIZE */
-heap   0x8000  /* HEAP AREA SIZE      */

/*-------------------------------------------*/
/*       Stack Sizes for various modes       */
/*-------------------------------------------*/
__IRQ_STACK_SIZE   = 0x1000;
__FIQ_STACK_SIZE   = 0x0100;
__ABORT_STACK_SIZE = 0x0100;
__UND_STACK_SIZE   = 0x0100;
__SVC_STACK_SIZE   = 0x0100;

/*--------------------------------------------------------------------------*/
/*                               Memory Map                                 */
/*--------------------------------------------------------------------------*/
MEMORY
{
    /*=================== MCU R5F TCM Local View ======================*/
    MCU_R5F_TCMA_VECS       (X) : ORIGIN = 0x00000000 LENGTH = 0x00000040
    MCU_R5F_TCMA            (X) : ORIGIN = 0x00000040 LENGTH = 0x00007FC0
    MCU_R5F_TCMB_VECS       (X) : ORIGIN = 0x41010000 LENGTH = 0x00000040
    MCU_R5F_TCMB            (X) : ORIGIN = 0x41010040 LENGTH = 0x00007FC0

    /*==================== MCU R5F TCM SOC View =======================*/
    /*---------------------- MCU R5FSS0 CORE0 -------------------------*/
    MCU1_R5F0_ATCM_RSVD  (RWIX) : ORIGIN = 0x41000000 LENGTH = 0x00000040
    MCU1_R5F0_ATCM       (RWIX) : ORIGIN = 0x41000040 LENGTH = 0x00007FC0
 // MCU1_R5F0_BTCM_RSVD  (RWIX) : ORIGIN = 0x41010000 LENGTH = 0x00000040 (Documented only, to avoid conflict above with MCU_R5F_TCMB_VECS) 
 // MCU1_R5F0_BTCM       (RWIX) : ORIGIN = 0x41010040 LENGTH = 0x00007FC0 (Documented only, to avoid conflict above with MCU_R5F_TCMB)
    /*---------------------- MCU R5FSS0 CORE1 -------------------------*/
    MCU1_R5F1_ATCM_RSVD  (RWIX) : ORIGIN = 0x41400000 LENGTH = 0x00000040
    MCU1_R5F1_ATCM       (RWIX) : ORIGIN = 0x41400040 LENGTH = 0x00007FC0
    MCU1_R5F1_BTCM_RSVD  (RWIX) : ORIGIN = 0x41410000 LENGTH = 0x00000040
    MCU1_R5F1_BTCM       (RWIX) : ORIGIN = 0x41410040 LENGTH = 0x00007FC0

    /*===================== MCU MSRAM Locations =======================*/
    OCMRAM_MCU1_0_VECS   (RWIX) : ORIGIN = 0x41C3E000 LENGTH = 0x00000040
    OCMRAM_MCU1_0        (RWIX) : ORIGIN = 0x41C3E040 LENGTH = 0x00000FC0
    OCMRAM_MCU1_1_VECS   (RWIX) : ORIGIN = 0x41C3F000 LENGTH = 0x00000040
    OCMRAM_MCU1_1        (RWIX) : ORIGIN = 0x41C3F040 LENGTH = 0x00000FC0

    /*=================== COMPUTE_CLUSTER0_MSMC_SRAM ==================*/
    MSMC3                (RWIX) : ORIGIN = 0x70000000 LENGTH = 0x001EFC00
    /*---------- The ORIGIN  and LENGTH  is determined by board cfg, ----------*/ 
    /*------------------ refer user guide for details -------------------------*/
    MSMC3_RSVD_DMSC      (RWIX) : ORIGIN = 0x701F0000 LENGTH = 0x00001000

    /*===================== J721E DDR Locations =======================*/
    /* DDR Memory Map is included from memory_map_ddr.cmd -------------*/
}

/*--------------------------------------------------------------*/
/*                     Section Configuration                    */
/*--------------------------------------------------------------*/
SECTIONS
{
    .freertosrstvectors : {} palign(8)      > __VECS
    
    .bootCode           : {} palign(8)      > __BOOT
    .startupCode        : {} palign(8)      > __BOOT
    .startupData        : {} palign(8)      > __BOOT, type = NOINIT
    GROUP 
    {
        .text.hwi       : palign(8)
        .text.cache     : palign(8)
        .text.mpu       : palign(8)
        .text.boot      : palign(8)
    }                                       > __BOOT

    .text               : {} palign(8)      > __CORE_DDR_SPACE
    .const              : {} palign(8)      > __CORE_DDR_SPACE
    .cinit              : {} palign(8)      > __CORE_DDR_SPACE
    .bss                : {} align(4)       > __CORE_DDR_SPACE
    .far                : {} align(4)       > __CORE_DDR_SPACE
    .data               : {} palign(128)    > __CORE_DDR_SPACE
    .sysmem             : {}                > __CORE_DDR_SPACE
    .data_buffer        : {} palign(128)    > __CORE_DDR_SPACE
    .bss.devgroup*      : {} align(4)       > __CORE_DDR_SPACE
    .const.devgroup*    : {} align(4)       > __CORE_DDR_SPACE
    .boardcfg_data      : {} align(4)       > __CORE_DDR_SPACE

    /* USB or any other LLD buffer for benchmarking */
    .benchmark_buffer (NOLOAD) {} align (8)     > __CORE_DDR_SPACE
    ipc_data_buffer (NOINIT) : {} palign(128)	> __CORE_DDR_SPACE
    .resource_table          : 
    {
        __RESOURCE_TABLE = .;
    }                                           > __CORE_EXT_DATA_BASE

    .tracebuf                : {} align(1024)   > __CORE_EXT_DATA

    .stack                   : {} align(4)      > __CORE_DDR_SPACE  (HIGH)

    .irqStack   : {. = . + __IRQ_STACK_SIZE;} align(4)      > __CORE_DDR_SPACE  (HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)

    .fiqStack   : {. = . + __FIQ_STACK_SIZE;} align(4)      > __CORE_DDR_SPACE  (HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)

    .abortStack : {. = . + __ABORT_STACK_SIZE;} align(4)    > __CORE_DDR_SPACE  (HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)

    .undStack   : {. = . + __UND_STACK_SIZE;} align(4)      > __CORE_DDR_SPACE  (HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)

    .svcStack   : {. = . + __SVC_STACK_SIZE;} align(4)      > __CORE_DDR_SPACE  (HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)
}
