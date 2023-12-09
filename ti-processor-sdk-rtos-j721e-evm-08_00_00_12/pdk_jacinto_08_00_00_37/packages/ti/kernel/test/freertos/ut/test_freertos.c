/*
 *  Copyright (C) 2018-2021 Texas Instruments Incorporated
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
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <ti/osal/osal.h>
#include <ti/osal/DebugP.h>
#include <ti/osal/HwiP.h>
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <unity.h>
#include <unity_config.h>
#include <ti/csl/soc.h>
#include <FREERTOS_log.h>

/* 
 * IMPORTANT NOTES: 
 * 
 * Alignment of stack is not strictly needed for R5F but debug is easier if stack is nicely 
 * aligned.
 * 
 * Task priority, 0 is lowest priority, configMAX_PRIORITIES-1 is highest
 * For this example any valid task priority can be set.
 * 
 * See FreeRTOSConfig.h for configMAX_PRIORITIES and StackType_t type.
 * FreeRTOSConfig.h can be found under kernel/freertos/config/${device}/${cpu}/
 * 
 * In this example, 
 * We create task's, semaphore's, ISR's and stack for the tasks using static allocation. 
 * We dont need to delete these semaphore's since static allocation is used.
 * 
 * One MUST not return out of a FreeRTOS task instead one MUST call vTaskDelete instead.
 */

#define NUM_TASK_SWITCHES (1000000u)

#if defined (SOC_TPR12) || defined (SOC_AWR294X)
    #if defined(BUILD_MCU1_0) || defined(BUILD_MCU1_1)
        #define PING_INT_NUM           (236u)
        #define PONG_INT_NUM           (237u)
    #endif
    #ifdef BUILD_C66X_1
        #define PING_INT_NUM           (8u)
        #define PING_EVT_ID            (CSL_DSS_INTR_DSS_RTIB_0)
        #define PONG_INT_NUM           (9u)
        #define PONG_EVT_ID            (CSL_DSS_INTR_DSS_RTIB_1)
    #endif
#endif

#ifdef SOC_AM65XX
    #if defined (BUILD_MCU1_0) || defined (BUILD_MCU1_1)
        #define PING_INT_NUM           (CSL_MCU0_INTR_MAIN2MCU_LVL_INTR0_OUTL_0)
        #define PONG_INT_NUM           (CSL_MCU0_INTR_MAIN2MCU_LVL_INTR0_OUTL_1)
    #endif
#endif

#ifdef SOC_J721E
    #ifdef BUILD_MCU1_0
        #define PING_INT_NUM           (CSLR_MCU_R5FSS0_CORE0_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_0)
        #define PONG_INT_NUM           (CSLR_MCU_R5FSS0_CORE0_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_1)
    #endif
    #ifdef BUILD_MCU1_1
        #define PING_INT_NUM           (CSLR_MCU_R5FSS0_CORE1_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_0)
        #define PONG_INT_NUM           (CSLR_MCU_R5FSS0_CORE1_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_1)
    #endif
    #ifdef BUILD_MCU2_0
        #define PING_INT_NUM           (CSLR_R5FSS0_CORE0_INTR_R5FSS0_INTROUTER0_OUTL_0)
        #define PONG_INT_NUM           (CSLR_R5FSS0_CORE0_INTR_R5FSS0_INTROUTER0_OUTL_1)
    #endif
    #ifdef BUILD_MCU2_1
        #define PING_INT_NUM           (CSLR_R5FSS0_CORE1_INTR_R5FSS0_INTROUTER0_OUTL_0)
        #define PONG_INT_NUM           (CSLR_R5FSS0_CORE1_INTR_R5FSS0_INTROUTER0_OUTL_1)
    #endif
    #ifdef BUILD_MCU3_0
        #define PING_INT_NUM           (CSLR_R5FSS1_CORE0_INTR_R5FSS1_INTROUTER0_OUTL_0)
        #define PONG_INT_NUM           (CSLR_R5FSS1_CORE0_INTR_R5FSS1_INTROUTER0_OUTL_1)
    #endif
    #ifdef BUILD_MCU3_1
        #define PING_INT_NUM           (CSLR_R5FSS1_CORE1_INTR_R5FSS1_INTROUTER0_OUTL_0)
        #define PONG_INT_NUM           (CSLR_R5FSS1_CORE1_INTR_R5FSS1_INTROUTER0_OUTL_1)
    #endif
    #ifdef BUILD_C66X_1
        #define PING_INT_NUM           (8u)
        #define PING_EVT_ID            (CSLR_C66SS0_CORE0_C66_EVENT_IN_SYNC_C66SS0_INTROUTER0_OUTL_0)
        #define PONG_INT_NUM           (9u)
        #define PONG_EVT_ID            (CSLR_C66SS0_CORE0_C66_EVENT_IN_SYNC_C66SS0_INTROUTER0_OUTL_1)
    #endif
    #ifdef BUILD_C66X_2
        #define PING_INT_NUM           (8u)
        #define PING_EVT_ID            (CSLR_C66SS1_CORE0_C66_EVENT_IN_SYNC_C66SS1_INTROUTER0_OUTL_0)
        #define PONG_INT_NUM           (9u)
        #define PONG_EVT_ID            (CSLR_C66SS1_CORE0_C66_EVENT_IN_SYNC_C66SS1_INTROUTER0_OUTL_1)
    #endif
#endif

#ifdef SOC_J7200
    #ifdef BUILD_MCU1_0
        #define PING_INT_NUM           (CSLR_MCU_R5FSS0_CORE0_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_0)
        #define PONG_INT_NUM           (CSLR_MCU_R5FSS0_CORE0_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_1)
    #endif
    #ifdef BUILD_MCU1_1
        #define PING_INT_NUM           (CSLR_MCU_R5FSS0_CORE1_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_0)
        #define PONG_INT_NUM           (CSLR_MCU_R5FSS0_CORE1_INTR_MAIN2MCU_LVL_INTRTR0_OUTL_1)
    #endif
    #ifdef BUILD_MCU2_0
        #define PING_INT_NUM           (CSLR_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_MSMC_EN_SOC_EVENTS_OUT_LEVEL_0)
        #define PONG_INT_NUM           (CSLR_R5FSS0_CORE0_INTR_COMPUTE_CLUSTER0_MSMC_EN_SOC_EVENTS_OUT_LEVEL_1)
    #endif
    #ifdef BUILD_MCU2_1
        #define PING_INT_NUM           (CSLR_R5FSS0_CORE1_INTR_COMPUTE_CLUSTER0_MSMC_EN_SOC_EVENTS_OUT_LEVEL_0)
        #define PONG_INT_NUM           (CSLR_R5FSS0_CORE1_INTR_COMPUTE_CLUSTER0_MSMC_EN_SOC_EVENTS_OUT_LEVEL_1)
    #endif
#endif

#define PING_TASK_PRI (2u)
#define PONG_TASK_PRI (3u)

#define PING_TASK_SIZE (16384u)
StackType_t gPingTaskStack[PING_TASK_SIZE] __attribute__((aligned(32)));

#define PONG_TASK_SIZE (16384u)
StackType_t gPongTaskStack[PONG_TASK_SIZE] __attribute__((aligned(32)));

StaticTask_t gPingTaskObj;
StaticSemaphore_t gPingSemObj;
TaskHandle_t gPingTask;
SemaphoreHandle_t gPingSem;

StaticTask_t gPongTaskObj;
StaticSemaphore_t gPongSemObj;
TaskHandle_t gPongTask;
SemaphoreHandle_t gPongSem;

static void ping_isr_1(uintptr_t arg)
{
    BaseType_t doTaskSwitch = 0;

    xSemaphoreGiveFromISR(gPingSem, &doTaskSwitch); /* wake up ping task */
    portYIELD_FROM_ISR(doTaskSwitch);
}

static void ping_isr_2(uintptr_t arg)
{
    BaseType_t doTaskSwitch = 0;

    xSemaphoreGiveFromISR(gPongSem, &doTaskSwitch); /* wake up pong task */
    portYIELD_FROM_ISR(doTaskSwitch);
}

static void ping_isr_3(uintptr_t arg)
{
    BaseType_t doTaskSwitch = 0;

    vTaskNotifyGiveFromISR(gPingTask, &doTaskSwitch); /* wake up ping task */
    portYIELD_FROM_ISR(doTaskSwitch);
}

static void ping_isr_4(uintptr_t arg)
{
    BaseType_t doTaskSwitch = 0;

    vTaskNotifyGiveFromISR(gPongTask, &doTaskSwitch); /* wake up pong task */
    portYIELD_FROM_ISR(doTaskSwitch);
}

static void pong_isr_2(uintptr_t arg)
{
    BaseType_t doTaskSwitch = 0;

    xSemaphoreGiveFromISR(gPingSem, &doTaskSwitch); /* wake up ping task */
    portYIELD_FROM_ISR(doTaskSwitch);
}

static void pong_isr_4(uintptr_t arg)
{
    BaseType_t doTaskSwitch = 0;

    vTaskNotifyGiveFromISR(gPingTask, &doTaskSwitch); /* wake up ping task */
    portYIELD_FROM_ISR(doTaskSwitch);
}

/* switch between ping and pong tasks using semaphores */
void test_taskSwitchWithSemaphore(void)
{
    uint32_t count; /* loop `count` times */
    volatile uint64_t curTime; /* time in units of usecs */

    FREERTOS_log("\r\n");
    FREERTOS_log("[FreeRTOS] ping task ... start !!!\r\n");
    count = NUM_TASK_SWITCHES;
    curTime = uiPortGetRunTimeCounterValue();
    while (count--)
    {
        xSemaphoreGive(gPongSem);                /* wake up pong task */
        xSemaphoreTake(gPingSem, portMAX_DELAY); /* wait for pong to signal */
    }
    curTime = uiPortGetRunTimeCounterValue() - curTime;

        FREERTOS_log("\r\n");
        FREERTOS_log("execution time for task switches = %d ms\r\n", (uint32_t)(curTime/1000));
        FREERTOS_log("number of task switches = %d \r\n", (uint32_t)NUM_TASK_SWITCHES*2);
        FREERTOS_log("time per task switch (semaphore give/take) = %d ns\r\n", (uint32_t)(curTime*1000/(NUM_TASK_SWITCHES*2)));
}

/* switch between ping and pong tasks using direct-to-task notifications */
void test_taskSwitchWithTaskNotify(void)
{
    uint32_t count; /* loop `count` times */
    volatile uint64_t curTime; /* time in units of usecs */

    count = NUM_TASK_SWITCHES;
    curTime = uiPortGetRunTimeCounterValue();
    while (count--)
    {
        xTaskNotifyGive(gPongTask);              /* wake up pong task */
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* wait for pong to signal */
    }
    curTime = uiPortGetRunTimeCounterValue() - curTime;

        FREERTOS_log("\r\n");
        FREERTOS_log("execution time for task switches = %d ms\r\n", (uint32_t)(curTime/1000));
        FREERTOS_log("number of task switches = %d \r\n", (uint32_t)NUM_TASK_SWITCHES*2);
        FREERTOS_log("time per task switch (direct-to-task notification give/take) = %d ns\r\n", (uint32_t)(curTime*1000/(NUM_TASK_SWITCHES*2)));
}

/* just invoke the task switch logic without any semaphores or direct-to-task notifications */
void test_taskYeild(void)
{
    uint32_t count; /* loop `count` times */
    volatile uint64_t curTime; /* time in units of usecs */

    count = NUM_TASK_SWITCHES;
    curTime = uiPortGetRunTimeCounterValue();
    while (count--)
    {
        taskYIELD();
    }
    curTime = uiPortGetRunTimeCounterValue() - curTime;

    FREERTOS_log("\r\n");
    FREERTOS_log("execution time for task yields = %d ms\r\n", (uint32_t)(curTime/1000));
    FREERTOS_log("number of task yields = %d \r\n", (uint32_t)NUM_TASK_SWITCHES);
    FREERTOS_log("time per task yield = %d ns\r\n", (uint32_t)(curTime * 1000 / (NUM_TASK_SWITCHES)));
}

/* switch from ping to ISR and back to the same task using semaphores, here there is no task switch */
void test_taskToIsrUsingSemaphoreAndNoTaskSwitch(void)
{
    uint32_t count; /* loop `count` times */
    volatile uint64_t curTime; /* time in units of usecs */
    HwiP_Params hwiParams;
    HwiP_Handle hHwi;
    HwiP_Status hwiStatus;

    HwiP_Params_init(&hwiParams);
/* Need to configure event Id for c66x due to eventCombiner */
#ifdef _TMS320C6X
    hwiParams.evtId = PING_EVT_ID;
#endif
    hHwi = HwiP_create(PING_INT_NUM, ping_isr_1, &hwiParams);
    DebugP_assert(hHwi != NULL);


    count = NUM_TASK_SWITCHES;
    curTime = uiPortGetRunTimeCounterValue();
    while (count--)
    {
        HwiP_post(PING_INT_NUM);
        xSemaphoreTake(gPingSem, portMAX_DELAY); /* wait for ISR to signal */
    }
    curTime = uiPortGetRunTimeCounterValue() - curTime;

    hwiStatus = HwiP_delete(hHwi);
    DebugP_assert(hwiStatus == HwiP_OK);

    FREERTOS_log("\r\n");
    FREERTOS_log("execution time for task - ISR - task switches = %d ms\r\n", (uint32_t)(curTime/1000));
    FREERTOS_log("number of task switches = %d \r\n", (uint32_t)NUM_TASK_SWITCHES);
    FREERTOS_log("time per task - ISR - task switch (semaphore give/take) = %d ns\r\n", (uint32_t)(curTime * 1000 / (NUM_TASK_SWITCHES)));
}

/* switch from ping to ISR and back to the same task using direct-to-task notify, here there is no task switch */
void test_taskToIsrUsingTaskNotifyAndNoTaskSwitch(void)
{
    uint32_t count; /* loop `count` times */
    volatile uint64_t curTime; /* time in units of usecs */
    HwiP_Params hwiParams;
    HwiP_Handle hHwi;
    HwiP_Status hwiStatus;

    HwiP_Params_init(&hwiParams);
#ifdef _TMS320C6X
    hwiParams.evtId = PING_EVT_ID;
#endif
    hHwi = HwiP_create(PING_INT_NUM, ping_isr_3, &hwiParams);
    DebugP_assert(hHwi != NULL);


    count = NUM_TASK_SWITCHES;
    curTime = uiPortGetRunTimeCounterValue();
    while (count--)
    {
        HwiP_post(PING_INT_NUM);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* wait for ISR to signal */
    }
    curTime = uiPortGetRunTimeCounterValue() - curTime;

    hwiStatus = HwiP_delete(hHwi);
    DebugP_assert(hwiStatus == HwiP_OK);

    FREERTOS_log("\r\n");
    FREERTOS_log("execution time for task - ISR - task switches = %d ms\r\n", (uint32_t)(curTime/1000));
    FREERTOS_log("number of task switches = %d \r\n", (uint32_t)NUM_TASK_SWITCHES);
    FREERTOS_log("time per task - ISR - task switch (direct-to-task notification give/take) = %d ns\r\n", (uint32_t)(curTime * 1000 / (NUM_TASK_SWITCHES)));
}

/* switch from ping task to ISR to pong task and back to ping task using semaphores, here there is a task switch */
void test_taskToIsrUsingSemaphoreAndWithTaskSwitch(void)
{
    uint32_t count; /* loop `count` times */
    volatile uint64_t curTime; /* time in units of usecs */
    HwiP_Params hwiParams;
    HwiP_Handle hHwi;
    HwiP_Status hwiStatus;

    HwiP_Params_init(&hwiParams);
#ifdef _TMS320C6X
    hwiParams.evtId = PING_EVT_ID;
#endif
    hHwi = HwiP_create(PING_INT_NUM, ping_isr_2, &hwiParams);
    DebugP_assert(hHwi != NULL);

    count = NUM_TASK_SWITCHES;
    curTime = uiPortGetRunTimeCounterValue();
    while (count--)
    {
        HwiP_post(PING_INT_NUM);
        xSemaphoreTake(gPingSem, portMAX_DELAY); /* wait for ISR to signal */
    }
    curTime = uiPortGetRunTimeCounterValue() - curTime;

    hwiStatus = HwiP_delete(hHwi);
    DebugP_assert(hwiStatus == HwiP_OK);

    FREERTOS_log("\r\n");
    FREERTOS_log("execution time for task - ISR - task - task switches = %d ms\r\n", (uint32_t)(curTime/1000));
    FREERTOS_log("number of ISRs = %d \r\n", (uint32_t)NUM_TASK_SWITCHES * 2);
    FREERTOS_log("time per task - ISR - task switch (semaphore give/take) = %d ns\r\n", (uint32_t)(curTime * 1000 / (2 * NUM_TASK_SWITCHES)));
}

/* switch from ping task to ISR to pong task and back to ping task using direct-to-task notify, here there is task switch */
void test_taskToIsrUsingTaskNotifyAndWithTaskSwitch(void)
{
    uint32_t count; /* loop `count` times */
    volatile uint64_t curTime; /* time in units of usecs */
    HwiP_Params hwiParams;
    HwiP_Handle hHwi;
    HwiP_Status hwiStatus;

    HwiP_Params_init(&hwiParams);
#ifdef _TMS320C6X
    hwiParams.evtId = PING_EVT_ID;
#endif
    hHwi = HwiP_create(PING_INT_NUM, ping_isr_4, &hwiParams);
    DebugP_assert(hHwi != NULL);

    count = NUM_TASK_SWITCHES;
    curTime = uiPortGetRunTimeCounterValue();
    while (count--)
    {
        HwiP_post(PING_INT_NUM);
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* wait for ISR to signal */
    }
    curTime = uiPortGetRunTimeCounterValue() - curTime;

    hwiStatus = HwiP_delete(hHwi);
    DebugP_assert(hwiStatus == HwiP_OK);

    FREERTOS_log("\r\n");
    FREERTOS_log("execution time for task - ISR - task switches = %d ms\r\n", (uint32_t)(curTime/1000));
    FREERTOS_log("number of task switches = %d \r\n", (uint32_t)NUM_TASK_SWITCHES * 2);
    FREERTOS_log("time per task - ISR - task switch (direct-to-task notification give/take) = %d ns\r\n", (uint32_t)(curTime * 1000 / (NUM_TASK_SWITCHES * 2)));
}

/* switch between ping and pong tasks and do float operations in between */
void test_taskSwitchWithFloatOperations(void)
{
    uint32_t count; /* loop `count` times */
    volatile uint64_t curTime; /* time in units of usecs */
    double f;

    /* Any task that uses the floating point unit MUST call portTASK_USES_FLOATING_POINT()
        * before any floating point instructions are executed. 
        */
    portTASK_USES_FLOATING_POINT();

    f = 0.0;
    count = NUM_TASK_SWITCHES;
    curTime = uiPortGetRunTimeCounterValue();
    while (count--)
    {
        f = f + 0.01;
        xSemaphoreGive(gPongSem);                /* wake up pong task */
        xSemaphoreTake(gPingSem, portMAX_DELAY); /* wait for pong to signal */
    }
    curTime = uiPortGetRunTimeCounterValue() - curTime;

    FREERTOS_log("\r\n");
    FREERTOS_log("execution time for task switches = %d ms\r\n", (uint32_t)(curTime/1000));
    FREERTOS_log("number of task switches = %d \r\n", (uint32_t)NUM_TASK_SWITCHES * 2);
    FREERTOS_log("time per task switch (semaphore give/take) = %d ns\r\n", (uint32_t)(curTime * 1000 / (NUM_TASK_SWITCHES * 2)));
    TEST_ASSERT_UINT32_WITHIN(1, (NUM_TASK_SWITCHES / 100), (uint32_t)f);
}

/* wait some msecs, this is just to show how delay API can be used, 
* there is no need to delay before deleting the task 
*/
void test_taskDelay(void)
{
    volatile uint64_t curTime; /* time in units of usecs */
    uint32_t delay1 = 100, delay2 = 110; /* in msecs */

    curTime = uiPortGetRunTimeCounterValue();
    /* convert to ticks before pass to vTaskDelay */
    vTaskDelay(delay1 / portTICK_PERIOD_MS);
    vTaskDelay(delay2 / portTICK_PERIOD_MS);
    curTime = uiPortGetRunTimeCounterValue() - curTime;
    TEST_ASSERT_UINT32_WITHIN(portTICK_PERIOD_MS * 1000, (delay1 + delay2) * 1000, (uint32_t)curTime);
}

void ping_main(void *args)
{
    FREERTOS_log("Starting freertos ut\n");
    UNITY_BEGIN();

    RUN_TEST(test_taskSwitchWithSemaphore, 1, NULL);
    RUN_TEST(test_taskSwitchWithTaskNotify, 2, NULL);
    RUN_TEST(test_taskYeild, 3, NULL);
    RUN_TEST(test_taskToIsrUsingSemaphoreAndNoTaskSwitch, 4, NULL);
    RUN_TEST(test_taskToIsrUsingTaskNotifyAndNoTaskSwitch, 5, NULL);
    RUN_TEST(test_taskToIsrUsingSemaphoreAndWithTaskSwitch, 6, NULL);
    RUN_TEST(test_taskToIsrUsingTaskNotifyAndWithTaskSwitch, 7, NULL);
    RUN_TEST(test_taskSwitchWithFloatOperations, 8, NULL);
    RUN_TEST(test_taskDelay, 9, NULL);

    UNITY_END();

    if (Unity.TestFailures == 0)
    {
        FREERTOS_log("\r\n");
        FREERTOS_log("All tests have passed!!\r\n");
    }
    /* One MUST not return out of a FreeRTOS task instead one MUST call vTaskDelete */
    vTaskDelete(NULL);
}

void pong_main(void *args)
{
    uint32_t count; /* loop `count` times */

    count = NUM_TASK_SWITCHES;
    while (count--)
    {
        xSemaphoreTake(gPongSem, portMAX_DELAY); /* wait for ping to signal */
        xSemaphoreGive(gPingSem);                /* wakeup ping task */
    }

    count = NUM_TASK_SWITCHES;
    while (count--)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* wait for ping to signal */
        xTaskNotifyGive(gPingTask);              /* wake up ping task */
    }
    {
        HwiP_Params hwiParams;
        HwiP_Handle hHwi;
        HwiP_Status hwiStatus;

        HwiP_Params_init(&hwiParams);
#ifdef _TMS320C6X
        hwiParams.evtId = PONG_EVT_ID;
#endif
        hHwi = HwiP_create(PONG_INT_NUM, pong_isr_2, &hwiParams);
        DebugP_assert(hHwi != NULL);

        count = NUM_TASK_SWITCHES;
        while (count--)
        {
            xSemaphoreTake(gPongSem, portMAX_DELAY); /* wait for ISR to signal */
            HwiP_post(PONG_INT_NUM);
        }
        hwiStatus = HwiP_delete(hHwi);
        DebugP_assert(hwiStatus == HwiP_OK);
    }
    {
        HwiP_Params hwiParams;
        HwiP_Handle hHwi;
        HwiP_Status hwiStatus;

        HwiP_Params_init(&hwiParams);
#ifdef _TMS320C6X
        hwiParams.evtId = PONG_EVT_ID;
#endif
        hHwi = HwiP_create(PONG_INT_NUM, pong_isr_4, &hwiParams);
        DebugP_assert(hHwi != NULL);

        count = NUM_TASK_SWITCHES;
        while (count--)
        {
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY); /* wait for ISR to signal */
            HwiP_post(PONG_INT_NUM);
        }
        hwiStatus = HwiP_delete(hHwi);
        DebugP_assert(hwiStatus == HwiP_OK);
    }
    { /* switch between ping and pong tasks and do float operations in between */
        double f;

        /* Any task that uses the floating point unit MUST call portTASK_USES_FLOATING_POINT()
         * before any floating point instructions are executed. 
         */
        portTASK_USES_FLOATING_POINT();

        f = 10.0;

        count = NUM_TASK_SWITCHES;
        while (count--)
        {
            f = f + 0.1;
            xSemaphoreTake(gPongSem, portMAX_DELAY); /* wait for ping to signal */
            xSemaphoreGive(gPingSem);                /* wakeup ping task */
        }
    }

    /* One MUST not return out of a FreeRTOS task instead one MUST call vTaskDelete */
    vTaskDelete(NULL);
}

void setUp(void)
{
}

void tearDown(void)
{
}

void test_freertos_main(void *args)
{
    /* Open drivers to open the UART driver for console */
    //Drivers_open();

    /* first create the semaphores */
    gPingSem = xSemaphoreCreateBinaryStatic(&gPingSemObj);
    configASSERT(gPingSem != NULL);
    gPongSem = xSemaphoreCreateBinaryStatic(&gPongSemObj);
    configASSERT(gPongSem != NULL);

    /* then create the tasks, order of task creation does not matter for this example */
    gPongTask = xTaskCreateStatic(pong_main,      /* Pointer to the function that implements the task. */
                                  "pong",         /* Text name for the task.  This is to facilitate debugging only. */
                                  PONG_TASK_SIZE, /* Stack depth in units of StackType_t typically uint32_t on 32b CPUs */
                                  NULL,           /* We are not using the task parameter. */
                                  PONG_TASK_PRI,  /* task priority, 0 is lowest priority, configMAX_PRIORITIES-1 is highest */
                                  gPongTaskStack, /* pointer to stack base */
                                  &gPongTaskObj); /* pointer to statically allocated task object memory */
    configASSERT(gPongTask != NULL);

    gPingTask = xTaskCreateStatic(ping_main,      /* Pointer to the function that implements the task. */
                                  "ping",         /* Text name for the task.  This is to facilitate debugging only. */
                                  PING_TASK_SIZE, /* Stack depth in units of StackType_t typically uint32_t on 32b CPUs */
                                  NULL,           /* We are not using the task parameter. */
                                  PING_TASK_PRI,  /* task priority, 0 is lowest priority, configMAX_PRIORITIES-1 is highest */
                                  gPingTaskStack, /* pointer to stack base */
                                  &gPingTaskObj); /* pointer to statically allocated task object memory */
    configASSERT(gPingTask != NULL);

    /* Dont close drivers to keep the UART driver open for console */
    /* Drivers_close(); */
}
