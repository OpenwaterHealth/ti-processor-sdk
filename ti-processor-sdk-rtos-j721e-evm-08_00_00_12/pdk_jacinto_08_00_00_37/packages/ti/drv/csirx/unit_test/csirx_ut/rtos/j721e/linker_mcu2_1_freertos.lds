/*----------------------------------------------------------------------------*/
/* File: k3m4_r5f_linker.cmd                                                  */
/* Description:																  */
/*    Link command file for j7 M4 MCU 0 view							  */
/*	  TI ARM Compiler version 15.12.3 LTS or later							  */
/*                                                                            */
/*    Platform: QT                                                            */
/* (c) Texas Instruments 2017, All rights reserved.                           */
/*----------------------------------------------------------------------------*/
/*  History:															      *'
/*    Aug 26th, 2016 Original version .......................... Loc Truong   */
/*    Aug 01th, 2017 new TCM mem map  .......................... Loc Truong   */
/*    Nov 07th, 2017 Changes for R5F Init Code.................. Vivek Dhande */
/*----------------------------------------------------------------------------*/
/* Linker Settings                                                            */
/* Standard linker options													  */
--retain="*(.intvecs)"
--retain="*(.intc_text)"
--retain="*(.rstvectors)"
--retain="*(.bootCode)"
--retain="*(.startupCode)"
--retain="*(.startupData)"
--retain="*(.irqStack)"
--retain="*(.fiqStack)"
--retain="*(.abortStack)"
--retain="*(.undStack)"
--retain="*(.svcStack)"
--retain="*(.image_data)"
--retain="*(.tb_argData)"

--fill_value=0
--stack_size=0x4000
--heap_size=0x8000
--entry_point=_freertosresetvectors

-stack  0x20000                              /* SOFTWARE STACK SIZE           */
-heap   0x20000                              /* HEAP AREA SIZE                */

/*-------------------------------------------*/
/*       Stack Sizes for various modes       */
/*-------------------------------------------*/
__IRQ_STACK_SIZE   = 0x1000;
__FIQ_STACK_SIZE   = 0x0100;
__ABORT_STACK_SIZE = 0x0100;
__UND_STACK_SIZE   = 0x0100;
__SVC_STACK_SIZE   = 0x0100;

/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */
MEMORY
{
	VECTORS (X)  			: origin=0x41C7F000 length=0x1000
    /*  Reset Vectors base address(RESET_VECTORS) should be 64 bytes aligned  */
	RESET_VECTORS (X)  			: origin=0x41C00000 length=0x100
    /* MCU0_R5F_0 local view 												  */
    MCU0_R5F_TCMA (X)		: origin=0x0			length=0x8000
    MCU0_R5F_TCMB0 (RWIX)	: origin=0x41010000	length=0x8000

    /* MCU0_R5F_1 SoC view													  */
    MCU0_R5F1_ATCM (RWIX)  	: origin=0x41400000 length=0x8000
    MCU0_R5F1_BTCM (RWIX) 	: origin=0x41410000 length=0x8000

    /* MCU0 share locations													  */
    OCMRAM 	(RWIX) 			: origin=0x41C00100 length=0x80000 - 0x1100	     /* ~510KB */

    /* J7 M4 locations													  */
    MSMC3	(RWIX)	 		: origin=0x70000000 length=0x200000			/* 2MB */
    DDR_START    (RWIX) 	: origin=0x80000000 length=0x100    	   /* 256 words */
    DDR0    (RWIX)  		: origin=0x80000100 length=0x1FFFFF00     /* 512MB - 1KB */

/* Additional memory settings	*/

}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .freertosrstvectors      : {} palign(8)      > VECTORS
    .bootCode        : {} palign(8)      > OCMRAM
    .startupCode     : {} palign(8)      > OCMRAM
    .startupData     : {} palign(8)      > OCMRAM, type = NOINIT
    .text            : {} palign(8)      > DDR0
    GROUP {
        .text.hwi    : palign(8)
        .text.cache  : palign(8)
        .text.mpu    : palign(8)
        .text.boot   : palign(8)
    }                                    > DDR0
    .const   	: {} palign(8) 		> OCMRAM
    .cinit   	: {} palign(8) 		> OCMRAM
    .pinit   	: {} palign(8) 		> OCMRAM
    .bss     	: {} align(4)  		> DDR0
    .data    	: {} palign(128) 	> DDR0
    .data_buffer: {} palign(128) 	> DDR0
    .image_data : {} palign(128)	> DDR0
    .tb_argData : {} palign(128)	> DDR_START
    .csirx_buffer_msmc       : {} palign(128)    > MSMC3
    .csirx_buffer_ddr        : {} palign(128)    > DDR0

	.sysmem  	: {} 				> DDR0
	.stack  	: {} align(4)		> DDR0    (HIGH)
	.irqStack  	: {. = . + __IRQ_STACK_SIZE;} align(4)		> DDR0  (HIGH)
    RUN_START(__IRQ_STACK_START)
    RUN_END(__IRQ_STACK_END)
    .fiqStack  	: {. = . + __FIQ_STACK_SIZE;} align(4)		> DDR0  (HIGH)
    RUN_START(__FIQ_STACK_START)
    RUN_END(__FIQ_STACK_END)
    .abortStack  	: {. = . + __ABORT_STACK_SIZE;} align(4)		> DDR0  (HIGH)
    RUN_START(__ABORT_STACK_START)
    RUN_END(__ABORT_STACK_END)
    .undStack  	: {. = . + __UND_STACK_SIZE;} align(4)		> DDR0  (HIGH)
    RUN_START(__UND_STACK_START)
    RUN_END(__UND_STACK_END)
    .svcStack  	: {. = . + __SVC_STACK_SIZE;} align(4)		> DDR0  (HIGH)
    RUN_START(__SVC_STACK_START)
    RUN_END(__SVC_STACK_END)

/* Additional sections settings 	*/

}  /* end of SECTIONS */

/*----------------------------------------------------------------------------*/
/* Misc linker settings                                                       */


/*-------------------------------- END ---------------------------------------*/
