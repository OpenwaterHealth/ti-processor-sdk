/*----------------------------------------------------------------------------*/
/* File: linker.cmd                                                           */
/* Description:								      */
/*    Link command file for Maxwell Multicore Testcase			      */
/*                                                                            */
/*    Platform: R5 Cores on AM65xx                                            */
/* (c) Texas Instruments 2018, All rights reserved.                           */
/*----------------------------------------------------------------------------*/

--entry_point=_sblTestResetVectors

MEMORY
{
    MSMC3_MCU1_CPU0	: origin=0x70080000 length=0x2000			/* 8KB */
    MSMC3_MCU1_CPU1	: origin=0x70082000 length=0x2000			/* 8KB */
    MSMC3_MCU2_CPU0	: origin=0x70084000 length=0x2000			/* 8KB */
    MSMC3_MCU2_CPU1	: origin=0x70086000 length=0x2000			/* 8KB */
    MSMC3_MCU3_CPU0	: origin=0x70088000 length=0x2000			/* 8KB */
    MSMC3_MCU3_CPU1	: origin=0x7008A000 length=0x2000			/* 8KB */
    MSMC3_DSP1_C66X	: origin=0x7008C000 length=0x2000			/* 8KB */
    MSMC3_DSP2_C66X	: origin=0x7008E000 length=0x2000			/* 8KB */
    MSMC3_DSP1_C7X	: origin=0x70090000 length=0x2000			/* 8KB */
    MSMC3_DSP2_C7X	: origin=0x70092000 length=0x2000			/* 8KB */
    MSMC3_MPU1_CPU0	: origin=0x70094000 length=0x2000			/* 8KB */
    MSMC3_MPU1_CPU1	: origin=0x70096000 length=0x2000			/* 8KB */
    MSMC3_MPU2_CPU0	: origin=0x70098000 length=0x2000			/* 8KB */
    MSMC3_MPU2_CPU1	: origin=0x7009A000 length=0x2000			/* 8KB */
}

SECTIONS
{
    .sbl_mcu_1_0_resetvector 	: {} palign(8) 		> MSMC3_MCU1_CPU0
    .sbl_mcu_1_1_resetvector 	: {} palign(8) 		> MSMC3_MCU1_CPU1
    .sbl_mcu_2_0_resetvector 	: {} palign(8) 		> MSMC3_MCU2_CPU0
    .sbl_mcu_2_1_resetvector 	: {} palign(8) 		> MSMC3_MCU2_CPU1
    .sbl_mcu_3_0_resetvector 	: {} palign(8) 		> MSMC3_MCU3_CPU0
    .sbl_mcu_3_1_resetvector 	: {} palign(8) 		> MSMC3_MCU3_CPU1
    .sbl_c66x_0_resetvector     : {} palign(8) 		> MSMC3_DSP1_C66X
    .sbl_c66x_1_resetvector     : {} palign(8) 		> MSMC3_DSP2_C66X
    .sbl_c7x_0_resetvector      : {} palign(8) 		> MSMC3_DSP1_C7X
    .sbl_c7x_1_resetvector      : {} palign(8) 		> MSMC3_DSP2_C7X
    .sbl_mpu_1_0_resetvector 	: {} palign(8) 		> MSMC3_MPU1_CPU0
    .sbl_mpu_1_1_resetvector 	: {} palign(8) 		> MSMC3_MPU1_CPU1
    .sbl_mpu_2_0_resetvector 	: {} palign(8) 		> MSMC3_MPU2_CPU0
    .sbl_mpu_2_1_resetvector 	: {} palign(8) 		> MSMC3_MPU2_CPU1

}

