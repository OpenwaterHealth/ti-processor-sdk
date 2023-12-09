/* =============================================================================
 *   Copyright (c) Texas Instruments Incorporated 2021
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Event.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Clock.h>
#include <xdc/runtime/Error.h>
#include <ti/sysbios/family/arm/v8a/Mmu.h>
#include <ti/sysbios/utils/Load.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>

/* CSL Header files */
#include <ti/csl/soc.h>
#include <ti/csl/arch/r5/csl_arm_r5.h>
#include <ti/csl/arch/r5/csl_arm_r5_pmu.h>
#include <ti/osal/HwiP.h>

#include <xdc/std.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Queue.h>
#include <ti/sysbios/hal/Seconds.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
/* OSAL Header files */
#include <ti/osal/osal.h>

/* BOARD Header files */
#include <ti/board/board.h>

/* UART Header files */
#include <ti/drv/uart/UART.h>

#ifndef UART_ENABLED

#ifdef BUILD_MCU2_0
#define UART0_ADDR ((int)0x02800000)
#else
#define UART0_ADDR  ((int)0x40A00000)
#endif

#define UART_RHR    ((int)0x00U)
#define UART_LSR    ((int)0x14U)
#define UART_RHR_REG_ADDR   ((volatile unsigned int *)(UART0_ADDR + UART_RHR))
#define UART_LSR_REG_ADDR   ((volatile unsigned int *)(UART0_ADDR + UART_LSR))

#ifdef BUILD_MCU2_0
#pragma CODE_SECTION(sbl_putc, ".sbl_mcu_2_0_resetvector")
#pragma CODE_SECTION(sbl_puts, ".sbl_mcu_2_0_resetvector")
#elif BUILD_MCU1_0
#pragma CODE_SECTION(sbl_putc, ".sbl_mcu_1_0_resetvector")
#pragma CODE_SECTION(sbl_puts, ".sbl_mcu_1_0_resetvector")
#endif

void sbl_putc(unsigned char c)
{
    while((*UART_LSR_REG_ADDR & 0x20) == 0);
    *UART_RHR_REG_ADDR = c;
}
void sbl_puts(char *str)
{
    for (; *str != '\0'; str++ )
        sbl_putc(*str);
}
#endif

#include <ti/drv/uart/UART_stdio.h>
static void AppUtils_Printf (const char *pcString, ...);
Board_STATUS Board_uartStdioInit(void);


#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200))) && !(defined (NO_SCISERVER))
#include <ti/drv/sciclient/sciserver_tirtos.h>
#endif

void setupSciServer(void);
/**< Test application stack size */
#define SETUP_SCISERVER_TASK_PRI_HIGH   (3 + 1)
/**< High Priority for SCI Server */
#define SETUP_SCISERVER_TASK_PRI_LOW    (1)
/**< High Priority for SCI Server */
#define ENABLE_SCISERVER
#undef ENABLE_SCICLIENT

/**< Initialize SCI Server, to process RM/PM Requests by other cores */

#define NUM_TASK 16
#define NUM_TEST 10
#define BUF_SIZE 2048
//#define MEM_CPY_OPER 2400000
#define MEM_CPY_OPER 500 // min > 10 msec
#define BUFFER_IN_USE 2


Types_FreqHz freq1;

/* task_calls is the number of random calls to the slave tasks for different
 * test cases. This can be used to controll the runtime of the code
*/
uint32_t task_calls = 500;

/* For each test case the  size of the memcpy buffer can be defined
 * individually. Lesser the value more is the number of cache misses per
 * second but reduced run time as the number of instructions executed
 * reduces and the memcpy instructions still remain in the cache */
uint32_t memcopy_size_arr[NUM_TEST] = {0, 50, 100, 200, 500, 750, 1000, 1250, \
                                        1500, BUF_SIZE};

/* Variable to pick up value from the memcopy_size_arr for each test*/
uint32_t memcopy_size = 0;

/* Number of times each slave task must repeat the operation.
 * This can be used the control the execution time of the code without much
 * impact on the cache misses
*/
uint32_t iter = 1;

/* Counter for the number of sysbios task switches that occur during the
 * execution of the code
*/
uint32_t num_switches = 0;

Queue_Handle myQ[NUM_TASK];
Task_Handle main_task[NUM_TASK];

/* Array to hold all the semaphores */
void *gSemaphoreHandle[NUM_TASK];
void *gSemaphoreHandle_task;

/* Array to hold the address of all the source buffers*/
uint32_t *buf[NUM_TASK];

/* Source buffers for all the memcpy operations. They can lie in the OCMC
 * or the same memory as the code like flash. The location can be changed
 * from the linker file
*/

uint32_t buf_0[BUF_SIZE] __attribute__((section(".buf_0"))) ;
uint32_t buf_1[BUF_SIZE] __attribute__((section(".buf_1"))) ;
uint32_t buf_2[BUF_SIZE] __attribute__((section(".buf_2"))) ;
uint32_t buf_3[BUF_SIZE] __attribute__((section(".buf_3"))) ;
uint32_t buf_4[BUF_SIZE] __attribute__((section(".buf_4"))) ;
uint32_t buf_5[BUF_SIZE] __attribute__((section(".buf_5"))) ;
uint32_t buf_6[BUF_SIZE] __attribute__((section(".buf_6"))) ;
uint32_t buf_7[BUF_SIZE] __attribute__((section(".buf_7"))) ;
uint32_t buf_8[BUF_SIZE] __attribute__((section(".buf_8"))) ;
uint32_t buf_9[BUF_SIZE] __attribute__((section(".buf_9"))) ;
uint32_t buf_10[BUF_SIZE] __attribute__((section(".buf_10"))) ;
uint32_t buf_11[BUF_SIZE] __attribute__((section(".buf_11"))) ;
uint32_t buf_12[BUF_SIZE] __attribute__((section(".buf_12"))) ;
uint32_t buf_13[BUF_SIZE] __attribute__((section(".buf_13"))) ;
uint32_t buf_14[BUF_SIZE] __attribute__((section(".buf_14"))) ;
uint32_t buf_15[BUF_SIZE] __attribute__((section(".buf_15"))) ;

/* The target buffer for all the memcpy operations */
uint32_t buf_ocmc[BUF_SIZE] __attribute__((section(".buf_cpy")));

/* Slave Task Function Definition. All the tasks are same
 * functionally. The only differnce is their location in the memory. They
 * are placed in the memory such that all of them occupy the same cache entry
 * in the 4-way cahce. The sections are defined in the linker cmd file*/

void SlaveTaskFxn_0(UArg a0, UArg a1)  __attribute__((section(".task_0")));
void SlaveTaskFxn_1(UArg a0, UArg a1)  __attribute__((section(".task_1")));
void SlaveTaskFxn_2(UArg a0, UArg a1)  __attribute__((section(".task_2")));
void SlaveTaskFxn_3(UArg a0, UArg a1)  __attribute__((section(".task_3")));
void SlaveTaskFxn_4(UArg a0, UArg a1)  __attribute__((section(".task_4")));
void SlaveTaskFxn_5(UArg a0, UArg a1)  __attribute__((section(".task_5")));
void SlaveTaskFxn_6(UArg a0, UArg a1)  __attribute__((section(".task_6")));
void SlaveTaskFxn_7(UArg a0, UArg a1)  __attribute__((section(".task_7")));
void SlaveTaskFxn_8(UArg a0, UArg a1)  __attribute__((section(".task_8")));
void SlaveTaskFxn_9(UArg a0, UArg a1)  __attribute__((section(".task_9")));
void SlaveTaskFxn_10(UArg a0, UArg a1)  __attribute__((section(".task_10")));
void SlaveTaskFxn_11(UArg a0, UArg a1)  __attribute__((section(".task_11")));
void SlaveTaskFxn_12(UArg a0, UArg a1)  __attribute__((section(".task_12")));
void SlaveTaskFxn_13(UArg a0, UArg a1)  __attribute__((section(".task_13")));
void SlaveTaskFxn_14(UArg a0, UArg a1)  __attribute__((section(".task_14")));
void SlaveTaskFxn_15(UArg a0, UArg a1)  __attribute__((section(".task_15")));




typedef struct
{
    Queue_Elem elem;
    uint32_t task_call_number;
}msg;

#ifdef MULTICORE
#define SHARED_MEM_ADDR 0xB0000000
uint32_t test_cnt = 1;
#endif

void multicore_wait()
{

#ifdef MULTICORE
#ifdef BUILD_MCU1_0
    /* Delay before setting the trigger */
    unsigned long long cnt = 0;
    AppUtils_Printf("Delay...");
    while (cnt++ < 10000000)
    {
        if ( (cnt%1000000) == 0 )
        {
            AppUtils_Printf("%d/10", (int)(cnt/1000000));
        }
    }
    /* Set the event trigger */
    *(unsigned int *)(SHARED_MEM_ADDR) = test_cnt;
#else
    AppUtils_Printf("Waiting for MCU1_0 to syncronize\n");
#endif

    uint32_t comparison_val = 0;
    while(comparison_val != test_cnt)
    {
        /* Reload the memory address and avoid compiler optimization */
        memcpy(&comparison_val, (unsigned int *)SHARED_MEM_ADDR, 1);
    }
    test_cnt++;
#endif
}

/* The fucntion definition for all the tasks.  */
#define TSKFN \
    msg* rp; \
    int i,j, arr_number,m; \
    while(1) \
    { \
        Semaphore_pend((Semaphore_Handle)gSemaphoreHandle[(uint32_t)a0], BIOS_WAIT_FOREVER); \
        while (!Queue_empty(myQ[(uint32_t)a0])) \
        { \
            rp = Queue_dequeue(myQ[(uint32_t)a0]); \
            for (i = 0; i < iter; ++i) \
            { \
                arr_number = get_rand() % BUFFER_IN_USE; \
                for (j = 0; j < memcopy_size; ++j) \
                { \
                    buf_ocmc[j] = buf[arr_number][j]; \
                } \
                /*sin calculation*/ \
                float denominator, sinx;  \
                float n = 30;        \
                n = n * (3.142 / 180.0);   \
                float x1 = n;  \
                sinx = n;           \
                m = 1;  \
                denominator = 2 * m * (2 * m + 1);  \
                x1 = -x1 * n * n / denominator;  \
                sinx = sinx + x1;  \
                m =  + 1;  \
                buf_ocmc[23] = buf_ocmc[23] + 1; \
                buf_ocmc[24] = buf_ocmc[24] + 1; \
                buf_ocmc[25] = buf_ocmc[25] + 1; \
                buf_ocmc[26] = buf_ocmc[26] + 1; \
                buf_ocmc[27] = buf_ocmc[27] + 1; \
                buf_ocmc[28] = buf_ocmc[28] + 1; \
                buf_ocmc[29] = buf_ocmc[29] + 1; \
                buf_ocmc[30] = buf_ocmc[30] + 1; \
                buf_ocmc[31] = buf_ocmc[31] + 1; \
                buf_ocmc[32] = buf_ocmc[32] + 1; \
                buf_ocmc[33] = buf_ocmc[33] + 1; \
                buf_ocmc[34] = buf_ocmc[34] + 1; \
                buf_ocmc[35] = buf_ocmc[35] + 1; \
                buf_ocmc[36] = buf_ocmc[36] + 1; \
                buf_ocmc[37] = buf_ocmc[37] + 1; \
                buf_ocmc[38] = buf_ocmc[38] + 1; \
                buf_ocmc[39] = buf_ocmc[39] + 1; \
                buf_ocmc[40] = buf_ocmc[40] + 1; \
                buf_ocmc[41] = buf_ocmc[41] + 1; \
                buf_ocmc[42] = buf_ocmc[42] + 1; \
                buf_ocmc[43] = buf_ocmc[43] + 1; \
                buf_ocmc[44] = buf_ocmc[44] + 1; \
                buf_ocmc[45] = buf_ocmc[45] + 1; \
                buf_ocmc[46] = buf_ocmc[46] + 1; \
                buf_ocmc[47] = buf_ocmc[47] + 1; \
                buf_ocmc[48] = buf_ocmc[48] + 1; \
                buf_ocmc[49] = buf_ocmc[49] + 1; \
                buf_ocmc[50] = buf_ocmc[50] + 1; \
                buf_ocmc[51] = buf_ocmc[51] + 1; \
                buf_ocmc[52] = buf_ocmc[52] + 1; \
                buf_ocmc[53] = buf_ocmc[53] + 1; \
                buf_ocmc[54] = buf_ocmc[54] + 1; \
                buf_ocmc[55] = buf_ocmc[55] + 1; \
                buf_ocmc[56] = buf_ocmc[56] + 1; \
                buf_ocmc[57] = buf_ocmc[57] + 1; \
                buf_ocmc[58] = buf_ocmc[58] + 1; \
                denominator = 2 * m * (2 * m + 1);  \
                x1 = -x1 * n * n / denominator;  \
                sinx = sinx + x1;  \
                m =  + 1;  \
                buf_ocmc[53] = buf_ocmc[53] + 1; \
                buf_ocmc[54] = buf_ocmc[54] + 1; \
                buf_ocmc[55] = buf_ocmc[55] + 1; \
                buf_ocmc[56] = buf_ocmc[56] + 1; \
                buf_ocmc[57] = buf_ocmc[57] + 1; \
                buf_ocmc[58] = buf_ocmc[58] + 1; \
                denominator = 2 * m * (2 * m + 1);  \
                x1 = -x1 * n * n / denominator;  \
                sinx = sinx + x1;  \
                m =  + 1;  \
                buf_ocmc[53] = buf_ocmc[53] + 1; \
                buf_ocmc[54] = buf_ocmc[54] + 1; \
                buf_ocmc[55] = buf_ocmc[55] + 1; \
                buf_ocmc[56] = buf_ocmc[56] + 1; \
                buf_ocmc[57] = buf_ocmc[57] + 1; \
                buf_ocmc[58] = buf_ocmc[58] + 1; \
                denominator = 2 * m * (2 * m + 1);  \
                x1 = -x1 * n * n / denominator;  \
                sinx = sinx + x1;  \
                m =  + 1;  \
                buf_ocmc[53] = buf_ocmc[53] + 1; \
                buf_ocmc[54] = buf_ocmc[54] + 1; \
                buf_ocmc[55] = buf_ocmc[55] + 1; \
                buf_ocmc[56] = buf_ocmc[56] + 1; \
                buf_ocmc[57] = buf_ocmc[57] + 1; \
                buf_ocmc[58] = buf_ocmc[58] + 1; \
                x1 = -x1 * n * n / denominator;  \
                sinx = sinx + x1;  \
                m =  + 1;  \
                buf_ocmc[53] = buf_ocmc[53] + 1; \
                buf_ocmc[54] = buf_ocmc[54] + 1; \
                buf_ocmc[55] = buf_ocmc[55] + 1; \
                buf_ocmc[56] = buf_ocmc[56] + 1; \
                buf_ocmc[57] = buf_ocmc[57] + 1; \
                buf_ocmc[58] = buf_ocmc[58] + 1; \
            } \
            rp->task_call_number = rp->task_call_number + 1; \
        } \
        Semaphore_post((Semaphore_Handle)gSemaphoreHandle[(uint32_t)a0]); \
        Task_yield(); \
    }

/* The hook function defiend to be called at every task switch to count the
 * number of task switches per test  case
*/
void myswitchFxn(Task_Handle prev, Task_Handle next)
{
    num_switches++;
}

/* Get a random number depending upon the number of tasks*/
uint32_t get_rand()
{
    return (rand()%NUM_TASK);
}

uint64_t temp1, temp2;
uint64_t start = 0;
uint64_t end = 0;
uint64_t exec_time_bios = 0;
Types_Timestamp64 start_cnt_bios, end_cnt_bios; 

/* Master task which will call the slave tasks randomly and */
void MasterTask(UArg a0, UArg a1)
{
    AppUtils_Printf("\n\rmaster_task\n\r");
    uint32_t icm, ica, icnt;
    msg r[NUM_TASK];
    int i, j;
    uint32_t set = 0, way = 0;


    for (i = 0; i < NUM_TASK; ++i)
    {
        myQ[i] = Queue_create(NULL, NULL);
    }

    AppUtils_Printf("\n\rmaster_task -- start sending\n\r");

    uint32_t count = 0;
    /* this loop is for NUM_TEST with the specified task calls and memcpy_size*/
    for (i = 0; i < NUM_TEST; ++i)
    {
        /* counter to track the task calls already made*/
        count = 0;
        /*reset the value of task switch for that test*/
        num_switches = 0;
        /* number of times each task should repeat its operation*/
        iter = 1;

        /* size of the memcpy to be performaed by each task*/
        memcopy_size = memcopy_size_arr[i];
        /*invalidate all the cache to get fresh and relaible data*/
	    for (set = 0; set < 64; set ++)
        {
            for (way = 0; way < 4; way++)
            {
                CSL_armR5CacheCleanInvalidateDcacheSetWay(set, way);
            }
        }

        /* Wait for the cores to sync up (empty function if MULTICORE is not defined) */
        multicore_wait();

        CSL_armR5CacheInvalidateAllIcache();

        /* reset the PMU counters to get relevant data */
        CSL_armR5PmuResetCntrs();
        Timestamp_get64(&start_cnt_bios);

        /*start sending signals and messages to tasks*/
        while(count < task_calls)
        {
            /* Get a random task number*/
            j = get_rand();
            r[j].task_call_number = count++;
            /* Add the message to the queue of the task */
            Queue_enqueue(myQ[j], &(r[j].elem));
            /* Signal the task to start executing */
            Semaphore_post((Semaphore_Handle)gSemaphoreHandle[j]);
            /* Yield the CPU for the other task to execute*/
            Task_yield();
            /* Wait for the task to complete */
            Semaphore_pend((Semaphore_Handle)gSemaphoreHandle[j], BIOS_WAIT_FOREVER);
        }

        Timestamp_get64(&end_cnt_bios);

        temp1 = start_cnt_bios.hi * (1ULL << 32);
        start = temp1 + start_cnt_bios.lo;

        temp2 = end_cnt_bios.hi * (1ULL << 32);
        end = temp2 + end_cnt_bios.lo;

        exec_time_bios = ((end - start) * 1000000) / freq1.lo;

        /* Read the value of the PMU counteres and print the result */
        icm = CSL_armR5PmuReadCntr(0);
        ica = CSL_armR5PmuReadCntr(2);
        // dcm = CSL_armR5PmuReadCntr(1);
	    icnt = CSL_armR5PmuReadCntr(1);
        AppUtils_Printf("\nMem Cpy size    => %u\n", memcopy_size);
        AppUtils_Printf("Exec Time in usec => %u\n", (uint32_t)exec_time_bios);
        AppUtils_Printf("Iter            => %u\n", iter);
        AppUtils_Printf("Task calls      => %u\n", task_calls);
        AppUtils_Printf("Inst Cache miss => %u\n", icm);
        AppUtils_Printf("Inst Cache acc  => %u\n", ica);
        // AppUtils_Printf("Data Cache miss => %u\n", dcm);
        AppUtils_Printf("num switches    => %u\n", num_switches);
        AppUtils_Printf("num instr exec  => %u\n", icnt);
        AppUtils_Printf("ICM/sec         => %llu\n", ((uint64_t)icm*1000000)/(uint32_t)exec_time_bios);
        AppUtils_Printf("INST/sec        => %llu\n", ((uint64_t)icnt*1000000)/(uint32_t)exec_time_bios);
        AppUtils_Printf("ICM Percentage  => %.3f\n", (float) (((float) icm)/((float) icnt))*100);
    }

    AppUtils_Printf("\nTest finished.\n");
    BIOS_exit(0);
}

int do_main(void)
{

    /* Initialize SCI Client Server */
    /* this will be an empty function if we are not using mcu1_0 (or j721e/j7200) */
    setupSciServer();

    /* refer to r5 csl for PMU API references*/
    CSL_armR5PmuCfg(0, 0 ,1);
    CSL_armR5PmuEnableAllCntrs(1);

    CSL_armR5PmuCfgCntr(0, CSL_ARM_R5_PMU_EVENT_TYPE_ICACHE_MISS);
    CSL_armR5PmuCfgCntr(1, CSL_ARM_R5_PMU_EVENT_TYPE_I_X);
    CSL_armR5PmuCfgCntr(2, CSL_ARM_R5_PMU_EVENT_TYPE_ICACHE_ACCESS);

    CSL_armR5PmuEnableCntrOverflowIntr(0, 0);
    CSL_armR5PmuEnableCntrOverflowIntr(1, 0);
    CSL_armR5PmuEnableCntrOverflowIntr(2, 0);

    CSL_armR5PmuResetCntrs();

    CSL_armR5PmuEnableCntr(0, 1);
    CSL_armR5PmuEnableCntr(1, 1);
    CSL_armR5PmuEnableCntr(2, 1);

    uint32_t count0 = CSL_armR5PmuReadCntr(0);
    uint32_t count1 = CSL_armR5PmuReadCntr(1);

    int i,j;


    Board_STATUS boardInitStatus = 0;

#if defined(UART_ENABLED)
    Board_initCfg cfg = BOARD_INIT_UART_STDIO | BOARD_INIT_PINMUX_CONFIG_MCU | BOARD_INIT_MODULE_CLOCK_MCU;
    /* Use below when trying to use CCS to boot MCU2_0 from DDR or MSMC */
    boardInitStatus = Board_init(cfg);
    //Board_initCfg cfg = BOARD_INIT_UART_STDIO | BOARD_INIT_PINMUX_CONFIG | BOARD_INIT_MODULE_CLOCK;
#else
    boardInitStatus = Board_uartStdioInit();
#endif

    if (boardInitStatus != BOARD_SOK)
    {
        AppUtils_Printf("\nBoard_init failure\n");
        return(0);
    }
    AppUtils_Printf("\nBoard_init success\n");

    //AppUtils_Printf("Inst Cache Miss: %u\n", CSL_armR5PmuReadCntr(0));
    //AppUtils_Printf("Inst Cache Access: %u\n", CSL_armR5PmuReadCntr(2));
    //AppUtils_Printf("Data Cache Miss: %u\n", CSL_armR5PmuReadCntr(1));

    /*task Function pointer array*/
    Task_FuncPtr tasks[NUM_TASK];
    tasks[0] = SlaveTaskFxn_0;
    tasks[1] = SlaveTaskFxn_1;
    tasks[2] = SlaveTaskFxn_2;
    tasks[3] = SlaveTaskFxn_3;
    tasks[4] = SlaveTaskFxn_4;
    tasks[5] = SlaveTaskFxn_5;
    tasks[6] = SlaveTaskFxn_6;
    tasks[7] = SlaveTaskFxn_7;
    tasks[8] = SlaveTaskFxn_8;
    tasks[9] = SlaveTaskFxn_9;
    tasks[10] = SlaveTaskFxn_10;
    tasks[11] = SlaveTaskFxn_11;
    tasks[12] = SlaveTaskFxn_12;
    tasks[13] = SlaveTaskFxn_13;
    tasks[14] = SlaveTaskFxn_14;
    tasks[15] = SlaveTaskFxn_15;

    /* array of buffers to be populated */
    buf[0] = buf_0;
    buf[1] = buf_1;
    buf[2] = buf_2;
    buf[3] = buf_3;
    buf[4] = buf_4;
    buf[5] = buf_5;
    buf[6] = buf_6;
    buf[7] = buf_7;
    buf[8] = buf_8;
    buf[9] = buf_9;
    buf[10] = buf_10;
    buf[11] = buf_11;
    buf[12] = buf_12;
    buf[13] = buf_13;
    buf[14] = buf_14;
    buf[15] = buf_15;

    /* Filling up the buffers with if they do not lie in the flash */
    AppUtils_Printf("Filling up the buffers\n");
    for (i = 0; i < NUM_TASK; ++i)
    {
        for (j = 0; j < BUF_SIZE; ++j)
        {
            buf[i][j] = j;
        }
    }

    /* Creating a task parameter */
    Task_Params taskParams;
    /* populating it with default values*/
    Task_Params_init(&taskParams);
    /* all tasks will have same priority*/
    taskParams.priority = 1;
    taskParams.stackSize = 0x500;

    /* creating master and slacve tasks*/
    main_task[0] = Task_create (MasterTask, &taskParams, NULL);
    for (i = 0; i < NUM_TASK; ++i)
    {
        taskParams.arg0 = (uint32_t)i;
        main_task[i] = Task_create(tasks[i], &taskParams, NULL);
    }
    /* initializing the semaphores*/
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    for (i = 0; i < NUM_TASK; ++i)
    {
        gSemaphoreHandle[i] = ((void *)Semaphore_create(0, &semParams, NULL));
    }

    Timestamp_getFreq(&freq1);
    AppUtils_Printf("BIOS Start freq.lo: %u\n", freq1.lo);

    CSL_armR5PmuResetCntrs();
    uint32_t Val0 = CSL_armR5PmuReadCntr(0);
    uint32_t Val2 = CSL_armR5PmuReadCntr(2);
    uint32_t Val1 = CSL_armR5PmuReadCntr(1);

    /* Sanity checking for PMU counters*/
    AppUtils_Printf("Inst Cache Miss: %u\n", Val0);
    AppUtils_Printf("Inst Cache Access: %u\n", Val2);
    AppUtils_Printf("Data Cache Miss: %u\n", Val1);

    /* Start the BIOS tasks*/
    BIOS_start ();
    return 0;
}

int main(void)
{
#ifdef BUILD_MCU2_0
    extern const UInt32 ti_sysbios_family_arm_v7r_keystone3_Hwi_vectors[];
 
    int i = 0;
 
    for(i = 0; i < 0x100; i = i + 4)
 
    {
 
        HWREG(i) = HWREG((uint8_t *)ti_sysbios_family_arm_v7r_keystone3_Hwi_vectors + i);
 
    }
#endif

#ifdef MULTICORE
#ifdef BUILD_MCU1_0
    AppUtils_Printf("Clearing the multicore register\n");
    *(unsigned int *)(SHARED_MEM_ADDR) = 0x0;
#endif
#endif

    do_main();
}

void SlaveTaskFxn_0(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_1(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_2(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_3(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_4(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_5(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_6(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_7(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_8(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_9(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_10(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_11(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_12(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_13(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_14(UArg a0, UArg a1)
{TSKFN}

void SlaveTaskFxn_15(UArg a0, UArg a1)
{TSKFN}


/**
 *  \brief Printf utility
 *
 */
#define APP_UTILS_PRINT_MAX_STRING_SIZE (2000U)
void AppUtils_Printf (const char *pcString, ...)
{
    static char printBuffer[APP_UTILS_PRINT_MAX_STRING_SIZE];
    va_list arguments;

    /* Start the varargs processing. */
    va_start(arguments, pcString);
    vsnprintf (printBuffer, sizeof(printBuffer), pcString, arguments);

    {
/* UART Prints do not work in XIP mode */
#ifdef UART_ENABLED
        UART_printf("%s",printBuffer);
#elif defined(CCS)
        printf("%s", printBuffer);
#else
        sbl_puts("\r");
        sbl_puts(printBuffer);
        sbl_puts("\n");
#endif
    }
    /* End the varargs processing. */
    va_end(arguments);

    return;
}


void setupSciServer(void)
{

#if (defined (BUILD_MCU1_0) && (defined (SOC_J721E) || defined (SOC_J7200))) && !(defined (NO_SCISERVER))
    Sciserver_TirtosCfgPrms_t appPrms;
    int32_t ret = CSL_PASS;

    ret = Sciserver_tirtosInitPrms_Init(&appPrms);

    appPrms.taskPriority[SCISERVER_TASK_USER_LO] =
                                            SETUP_SCISERVER_TASK_PRI_LOW;
    appPrms.taskPriority[SCISERVER_TASK_USER_HI] =
                                            SETUP_SCISERVER_TASK_PRI_HIGH;

    if (ret == CSL_PASS)
    {
        ret = Sciserver_tirtosInit(&appPrms);
    }

    if (ret == CSL_PASS)
    {
        System_printf("Starting Sciserver..... PASSED\n");
    }
    else
    {
        System_printf("Starting Sciserver..... FAILED\n");
    }

#endif
    return;
}
