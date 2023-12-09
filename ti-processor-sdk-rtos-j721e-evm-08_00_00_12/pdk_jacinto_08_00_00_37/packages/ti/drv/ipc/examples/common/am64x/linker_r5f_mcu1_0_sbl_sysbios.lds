/* linker options */
--fill_value=0
--stack_size=0x2000
--heap_size=0x1000

-stack  0x2000                              /* SOFTWARE STACK SIZE           */
-heap   0x2000                              /* HEAP AREA SIZE                */

#define DDR0_ALLOCATED_START 0xA0000000

#define MCU1_0_EXT_DATA_BASE     (DDR0_ALLOCATED_START + 0x00100000)
#define MCU1_0_R5F_MEM_TEXT_BASE (DDR0_ALLOCATED_START + 0x00200000)
#define MCU1_0_R5F_MEM_DATA_BASE (DDR0_ALLOCATED_START + 0x00300000)
#define MCU1_0_DDR_SPACE_BASE    (DDR0_ALLOCATED_START + 0x00400000)

#define MCU1_1_ALLOCATED_START   DDR0_ALLOCATED_START + 0x01000000
#define MCU1_1_EXT_DATA_BASE     (MCU1_1_ALLOCATED_START + 0x00100000)
#define MCU1_1_R5F_MEM_TEXT_BASE (MCU1_1_ALLOCATED_START + 0x00200000)
#define MCU1_1_R5F_MEM_DATA_BASE (MCU1_1_ALLOCATED_START + 0x00300000)
#define MCU1_1_DDR_SPACE_BASE    (MCU1_1_ALLOCATED_START + 0x00400000)

#define MCU2_0_ALLOCATED_START   DDR0_ALLOCATED_START + 0x02000000
#define MCU2_0_EXT_DATA_BASE     (MCU2_0_ALLOCATED_START + 0x00100000)
#define MCU2_0_R5F_MEM_TEXT_BASE (MCU2_0_ALLOCATED_START + 0x00200000)
#define MCU2_0_R5F_MEM_DATA_BASE (MCU2_0_ALLOCATED_START + 0x00300000)
#define MCU2_0_DDR_SPACE_BASE    (MCU2_0_ALLOCATED_START + 0x00400000)

#define MCU2_1_ALLOCATED_START   DDR0_ALLOCATED_START + 0x03000000
#define MCU2_1_EXT_DATA_BASE     (MCU2_1_ALLOCATED_START + 0x00100000)
#define MCU2_1_R5F_MEM_TEXT_BASE (MCU2_1_ALLOCATED_START + 0x00200000)
#define MCU2_1_R5F_MEM_DATA_BASE (MCU2_1_ALLOCATED_START + 0x00300000)
#define MCU2_1_DDR_SPACE_BASE    (MCU2_1_ALLOCATED_START + 0x00400000)

#define ATCM_START 0x00000000
#define BTCM_START 0x41010000

-e __VECS_ENTRY_POINT
--retain="*(.utilsCopyVecsToAtcm)"

/*----------------------------------------------------------------------------*/
/* Memory Map                                                                 */
MEMORY
{
    /* Reserved for SBL code/data */
    SBL_RSVD (X)            : origin=0x70000000 length=0x80000
    
    /*  Reset Vectors base address(RESET_VECTORS) should be 64 bytes aligned  */
    RESET_VECTORS (X)       : origin=0x70080000 length=0x100
    /* am64x MCMS3 locations */
    MSMC3   (RWIX)          : origin=0x70080100 length=0x40000 - 0x1100

    DDR0_RESERVED    (RWIX)  	: origin=0x80000000 length=0x20000000	  	/* 512MB */
    MCU1_0_IPC_DATA (RWIX)	: origin=DDR0_ALLOCATED_START     length=0x00100000	/*   1MB */
    MCU1_0_EXT_DATA  (RWIX)	: origin=MCU1_0_EXT_DATA_BASE     length=0x00100000	/*   1MB */
    MCU1_0_R5F_MEM_TEXT (RWIX)	: origin=MCU1_0_R5F_MEM_TEXT_BASE length=0x00100000	/*   1MB */
    MCU1_0_R5F_MEM_DATA (RWIX)	: origin=MCU1_0_R5F_MEM_DATA_BASE length=0x00100000	/*   1MB */
    MCU1_0_DDR_SPACE (RWIX)	: origin=MCU1_0_DDR_SPACE_BASE    length=0x00C00000	/*  12MB */
    MCU1_1_IPC_DATA (RWIX)	: origin=MCU1_1_ALLOCATED_START   length=0x00100000	/*   1MB */
    MCU1_1_EXT_DATA  (RWIX)	: origin=MCU1_1_EXT_DATA_BASE     length=0x00100000	/*   1MB */
    MCU1_1_R5F_MEM_TEXT (RWIX)	: origin=MCU1_1_R5F_MEM_TEXT_BASE length=0x00100000	/*   1MB */
    MCU1_1_R5F_MEM_DATA (RWIX)	: origin=MCU1_1_R5F_MEM_DATA_BASE length=0x00100000	/*   1MB */
    MCU1_1_DDR_SPACE (RWIX)	: origin=MCU1_1_DDR_SPACE_BASE    length=0x00C00000	/*  12MB */
    MCU2_0_IPC_DATA (RWIX)	: origin=MCU2_0_ALLOCATED_START   length=0x00100000	/*   1MB */
    MCU2_0_EXT_DATA  (RWIX)	: origin=MCU2_0_EXT_DATA_BASE     length=0x00100000	/*   1MB */
    MCU2_0_R5F_MEM_TEXT (RWIX)	: origin=MCU2_0_R5F_MEM_TEXT_BASE length=0x00100000	/*   1MB */
    MCU2_0_R5F_MEM_DATA (RWIX)	: origin=MCU2_0_R5F_MEM_DATA_BASE length=0x00100000	/*   1MB */
    MCU2_0_DDR_SPACE (RWIX)	: origin=MCU2_0_DDR_SPACE_BASE    length=0x00C00000	/*  12MB */
    MCU2_1_IPC_DATA (RWIX)	: origin=MCU2_1_ALLOCATED_START   length=0x00100000	/*   1MB */
    MCU2_1_EXT_DATA  (RWIX)	: origin=MCU2_1_EXT_DATA_BASE     length=0x00100000	/*   1MB */
    MCU2_1_R5F_MEM_TEXT (RWIX)	: origin=MCU2_1_R5F_MEM_TEXT_BASE length=0x00100000	/*   1MB */
    MCU2_1_R5F_MEM_DATA (RWIX)	: origin=MCU2_1_R5F_MEM_DATA_BASE length=0x00100000	/*   1MB */
    MCU2_1_DDR_SPACE (RWIX)	: origin=MCU2_1_DDR_SPACE_BASE    length=0x00C00000	/*  12MB */

    SHARED_DDR_SPACE (RWIX)	: origin=0xA5000000 length=0x00800000           /*  8MB */

}  /* end of MEMORY */

/*----------------------------------------------------------------------------*/
/* Section Configuration                                                      */

SECTIONS
{
    .vecs       : {
        __VECS_ENTRY_POINT = .;
    } palign(8) > RESET_VECTORS
    .text_boot {
        *boot.aer5f*<*boot.o*>(.text)
     }  palign(8)   > MSMC3
    .text:xdc_runtime_Startup_reset__I     : {} palign(8) > MSMC3
    .text:ti_sysbios_family_arm_v7r_Cache* : {} palign(8) > MSMC3
    .text:ti_sysbios_family_arm_MPU*       : {} palign(8) > MSMC3
    .utilsCopyVecsToAtcm                   : {} palign(8) > MSMC3

    .text	: {} palign(8)	    	> MCU1_0_DDR_SPACE
    .cinit   	: {} palign(8) 		> MCU1_0_DDR_SPACE
    .bss   	: {} palign(8) 		> MCU1_0_DDR_SPACE
    .far   	: {} palign(8) 		> MCU1_0_DDR_SPACE
    .const     	: {} align(4)  		> MCU1_0_DDR_SPACE
    .data    	: {} palign(128) 	> MCU1_0_DDR_SPACE
    .sysmem  	: {} align(8)		> MCU1_0_DDR_SPACE
    .stack	: {} align(4)		> MCU1_0_DDR_SPACE
    .data_buffer: {} palign(128) 	> MCU1_0_DDR_SPACE
    ipc_data_buffer (NOINIT) : {} palign(128)	> MCU1_0_DDR_SPACE
    .resource_table : {
        __RESOURCE_TABLE = .;
    } > MCU1_0_EXT_DATA_BASE

    .tracebuf   : {}			> MCU1_0_EXT_DATA

}  /* end of SECTIONS */

/*----------------------------------------------------------------------------*/
/* Misc linker settings                                                       */


/*-------------------------------- END ---------------------------------------*/
