/*----------------------------------------------------------------------------*/
/* File: linker.lds                                                           */
/* Description:								      */
/*    Link command file for Maxwell Multicore Testcase			      */
/*                                                                            */
/*    Platform: A53 cores on AM65xx                                           */
/* (c) Texas Instruments 2018, All rights reserved.                           */
/*----------------------------------------------------------------------------*/

ENTRY(_sblTestResetVectors);

MEMORY
{
    MSMC3_MCU1_CPU0	: ORIGIN = 0x000070080000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_MCU1_CPU1	: ORIGIN = 0x000070082000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_MCU2_CPU0	: ORIGIN = 0x000070084000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_MCU2_CPU1	: ORIGIN = 0x000070086000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_MCU3_CPU0	: ORIGIN = 0x000070088000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_MCU3_CPU1	: ORIGIN = 0x00007008A000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_DSP1_C66X 	: ORIGIN = 0x00007008C000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_DSP2_C66X 	: ORIGIN = 0x00007008E000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_DSP1_C7X 	: ORIGIN = 0x000070090000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_DSP2_C7X 	: ORIGIN = 0x000070092000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_MPU1_CPU0 	: ORIGIN = 0x000070094000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_MPU1_CPU1 	: ORIGIN = 0x000070096000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_MPU2_CPU0 	: ORIGIN = 0x000070098000, LENGTH = 0x00002000			/* 8KB */
    MSMC3_MPU2_CPU1	: ORIGIN = 0x00007009A000, LENGTH = 0x00002000			/* 8KB */
}

SECTIONS 
{
    .sbl_mcu_1_0_resetvector : {} > MSMC3_MCU1_CPU0
    .sbl_mcu_1_1_resetvector : {} > MSMC3_MCU1_CPU1
    .sbl_mcu_2_0_resetvector : {} > MSMC3_MCU2_CPU0
    .sbl_mcu_2_1_resetvector : {} > MSMC3_MCU2_CPU1
    .sbl_mcu_3_0_resetvector : {} > MSMC3_MCU3_CPU0
    .sbl_mcu_3_1_resetvector : {} > MSMC3_MCU3_CPU1
    .sbl_c66x_0_resetvector  : {} > MSMC3_DSP1_C66X
    .sbl_c66x_1_resetvector  : {} > MSMC3_DSP2_C66X
    .sbl_c7x_0_resetvector   : {} > MSMC3_DSP1_C7X
    .sbl_c7x_1_resetvector   : {} > MSMC3_DSP2_C7X
    .sbl_mpu_1_0_resetvector : {} > MSMC3_MPU1_CPU0
    .sbl_mpu_1_1_resetvector : {} > MSMC3_MPU1_CPU1
    .sbl_mpu_2_0_resetvector : {} > MSMC3_MPU2_CPU0
    .sbl_mpu_2_1_resetvector : {} > MSMC3_MPU2_CPU1
}
