/*----------------------------------------------------------------------------*/
/* File: linker_mcu1_0_msmc.lds                                               */
/* Description:                                                               */
/*    Link command file for J7200 MCU1_0 view                                 */
/*                                                                            */
/* (c) Texas Instruments 2021, All rights reserved.                           */
/*----------------------------------------------------------------------------*/
/*  History:                                                                  */
/*    Aug 26th, 2016 Original version .......................... Loc Truong   */
/*    Aug 01th, 2017 new TCM mem map  .......................... Loc Truong   */
/*    Nov 07th, 2017 Changes for R5F Init Code.................. Vivek Dhande */
/*    May 14th, 2021 running on J7VCL and HSM..... Caleb Robey & Karan Saxena */
/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */
/* Standard linker options                                                    */
--retain="*(.intvecs)"
--retain="*(.intc_text)"
--retain="*(.rstvectors)"
--fill_value=0
-e __VECS_ENTRY_POINT

/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */
--define FILL_PATTERN=0xFEAA55EF
--define FILL_LENGTH=0x100

/* It is CRUCIAL that the TASK_SIZE be 0x1000 to be sure that the whole cache is filled by
    each task when it is loaded */
--define TASK_SIZE=0x1000
--define TEXT_SIZE=0x26000
--define BUF_SIZE=0x2000

--define OCMCRAM_START=0x41C62000
--define MSMC_START=0x70000000
--define XIP_MCU1_0_START=0x501c0100
--define DDR_START=0x80000000

--define NUM_TASK=0x10

MEMORY
{
    MCU0_R5F_TCMB0 (RWIX)	 : origin=0x41010100	                 length=0x8000-0x100
    RESERVED (X)             : origin=0x41C3E000                     length=0x2000
    BUF0                     : origin=OCMCRAM_START + (BUF_SIZE*0)                    length=BUF_SIZE
    BUF1                     : origin=OCMCRAM_START + (BUF_SIZE*1)                    length=BUF_SIZE
    BUF2                     : origin=OCMCRAM_START + (BUF_SIZE*2)                    length=BUF_SIZE
    BUF3                     : origin=OCMCRAM_START + (BUF_SIZE*3)                    length=BUF_SIZE
    BUF4                     : origin=OCMCRAM_START + (BUF_SIZE*4)                    length=BUF_SIZE
    BUF5                     : origin=OCMCRAM_START + (BUF_SIZE*5)                    length=BUF_SIZE
    BUF6                     : origin=OCMCRAM_START + (BUF_SIZE*6)                    length=BUF_SIZE
    BUF7                     : origin=OCMCRAM_START + (BUF_SIZE*7)                    length=BUF_SIZE
    BUF8                     : origin=OCMCRAM_START + (BUF_SIZE*8)                    length=BUF_SIZE
    BUF9                     : origin=OCMCRAM_START + (BUF_SIZE*9)                    length=BUF_SIZE
    BUF10                    : origin=OCMCRAM_START + (BUF_SIZE*10)                   length=BUF_SIZE
    BUF11                    : origin=OCMCRAM_START + (BUF_SIZE*11)                   length=BUF_SIZE
    BUF12                    : origin=OCMCRAM_START + (BUF_SIZE*12)                   length=BUF_SIZE
    BUF13                    : origin=OCMCRAM_START + (BUF_SIZE*13)                   length=BUF_SIZE
    BUF14                    : origin=OCMCRAM_START + (BUF_SIZE*14)                   length=BUF_SIZE
    BUF15                    : origin=OCMCRAM_START + (BUF_SIZE*15)                   length=BUF_SIZE
    BUF_CPY                  : origin=OCMCRAM_START + (BUF_SIZE*16)                   length=BUF_SIZE
    RESET_VECTORS (X)        : origin=OCMCRAM_START + (BUF_SIZE*17)                   length=0x100
    OCMC_RAM (RWIX)          : origin=OCMCRAM_START + (BUF_SIZE*17) + 0x100           length=0x90000 - (BUF_SIZE*17) - 0x100

    TSK0                     : origin=MSMC_START + (TASK_SIZE*0)   length=TASK_SIZE
    TSK1                     : origin=MSMC_START + (TASK_SIZE*1)   length=TASK_SIZE
    TSK2                     : origin=MSMC_START + (TASK_SIZE*2)   length=TASK_SIZE
    TSK3                     : origin=MSMC_START + (TASK_SIZE*3)   length=TASK_SIZE
    TSK4                     : origin=MSMC_START + (TASK_SIZE*4)   length=TASK_SIZE
    TSK5                     : origin=MSMC_START + (TASK_SIZE*5)   length=TASK_SIZE
    TSK6                     : origin=MSMC_START + (TASK_SIZE*6)   length=TASK_SIZE
    TSK7                     : origin=MSMC_START + (TASK_SIZE*7)   length=TASK_SIZE
    TSK8                     : origin=MSMC_START + (TASK_SIZE*8)   length=TASK_SIZE
    TSK9                     : origin=MSMC_START + (TASK_SIZE*9)   length=TASK_SIZE
    TSK10                    : origin=MSMC_START + (TASK_SIZE*10)  length=TASK_SIZE
    TSK11                    : origin=MSMC_START + (TASK_SIZE*11)  length=TASK_SIZE
    TSK12                    : origin=MSMC_START + (TASK_SIZE*12)  length=TASK_SIZE
    TSK13                    : origin=MSMC_START + (TASK_SIZE*13)  length=TASK_SIZE
    TSK14                    : origin=MSMC_START + (TASK_SIZE*14)  length=TASK_SIZE
    TSK15                    : origin=MSMC_START + (TASK_SIZE*15)  length=TASK_SIZE
    TEXT_ARR                 : origin=MSMC_START + (TASK_SIZE*16)  length=TEXT_SIZE


    XIP_FLASH_VECS (X)       : origin=0x501c0000           length=0x40
	XIP_FLASH (RWIX)         : origin=0x501c0040           length=0x5C000 - 0x40
}

SECTIONS
{
    .vecs       : {
        __VECS_ENTRY_POINT = .;
    } palign(8) > RESET_VECTORS

	.text_boot {                                                                
        *boot.aer5f*<*boot.o*>(.text)                                           
    }  palign(8)   > MCU0_R5F_TCMB0

    .text:xdc_runtime_Startup_reset__I     : {} palign(8)      > MCU0_R5F_TCMB0
    .text:ti_sysbios_family_arm_v7r_Cache* : {} palign(8)      > MCU0_R5F_TCMB0
    .text:ti_sysbios_family_arm_MPU*       : {} palign(8)      > MCU0_R5F_TCMB0
    .utilsCopyVecsToAtcm                   : {} palign(8)      > MCU0_R5F_TCMB0 
    .sbl_mcu_1_0_resetvector               : {} palign(8)      > OCMC_RAM


    .text                                  : {} palign(8)      > TEXT_ARR
    .task_0                                : {} palign(8)      > TSK0
    .task_1                                : {} palign(8)      > TSK1
    .task_2                                : {} palign(8)      > TSK2
    .task_3                                : {} palign(8)      > TSK3
    .task_4                                : {} palign(8)      > TSK4
    .task_5                                : {} palign(8)      > TSK5
    .task_6                                : {} palign(8)      > TSK6
    .task_7                                : {} palign(8)      > TSK7
    .task_8                                : {} palign(8)      > TSK8
    .task_9                                : {} palign(8)      > TSK9
    .task_10                               : {} palign(8)      > TSK10
    .task_11                               : {} palign(8)      > TSK11
    .task_12                               : {} palign(8)      > TSK12
    .task_13                               : {} palign(8)      > TSK13
    .task_14                               : {} palign(8)      > TSK14
    .task_15                               : {} palign(8)      > TSK15
    .const.devgroup*                       : {} align(4)       > OCMC_RAM    
    .const                                 : {} palign(8)      > OCMC_RAM
    .cinit                                 : {} palign(8)      > OCMC_RAM
    .pinit                                 : {} palign(8)      > OCMC_RAM    
    .boardcfg_data                         : {} align(4)       > OCMC_RAM
    .bss                                   : {} align(4)       > OCMC_RAM
    .buf_cpy                               : {} align(4)       > BUF_CPY
    .data                                  : {} palign(128)    > OCMC_RAM
    .sysmem                                : {}                > OCMC_RAM
    .bss.devgroup*                         : {} align(4)       > OCMC_RAM
    .buf_0                                 : {} align(4)       > BUF0
    .buf_1                                 : {} align(4)       > BUF1
    .buf_2                                 : {} align(4)       > BUF2
    .buf_3                                 : {} align(4)       > BUF3
    .buf_4                                 : {} align(4)       > BUF4
    .buf_5                                 : {} align(4)       > BUF5
    .buf_6                                 : {} align(4)       > BUF6
    .buf_7                                 : {} align(4)       > BUF7
    .buf_8                                 : {} align(4)       > BUF8
    .buf_9                                 : {} align(4)       > BUF9
    .buf_10                                : {} align(4)       > BUF10
    .buf_11                                : {} align(4)       > BUF11
    .buf_12                                : {} align(4)       > BUF12
    .buf_13                                : {} align(4)       > BUF13
    .buf_14                                : {} align(4)       > BUF14
    .buf_15                                : {} align(4)       > BUF15
    .stack                                 : {} align(4)       > OCMC_RAM (HIGH)

}

