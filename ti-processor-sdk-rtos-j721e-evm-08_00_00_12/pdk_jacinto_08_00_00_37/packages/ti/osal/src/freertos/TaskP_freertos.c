/*
 * Copyright (c) 2018, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 *  ======== TaskP_tirtos.c ========
 */
#include <ti/osal/TaskP.h>

#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ti/csl/csl_types.h>
#include <ti/osal/osal.h>
#include <ti/osal/DebugP.h>
#include <ti/osal/soc/osal_soc.h>

#include <FreeRTOS.h>
#include <task.h>

TaskHandle_t TaskP_getFreertosHandle(TaskP_Handle handle);
uint32_t TaskP_getTaskId(TaskP_Handle handle);
extern void LoadP_addTask(TaskP_Handle handle, uint32_t tskId);
extern void LoadP_removeTask(uint32_t tskId);

/**
 * \brief Value to be used for lowest priority task 
 */
#define TaskP_PRIORITY_LOWEST       (0u)

/**
 * \brief Value to be used for highest priority task 
 */
#define TaskP_PRIORITY_HIGHEST      (configMAX_PRIORITIES - 1)

/*
 * Defines the prototype of the task main functions.
 */
typedef void (* TaskP_mainFunction_t)( void *arg0, void *arg1);

/*!
 *  @brief    Task structure
 */
typedef struct TaskP_freertos_s {
    bool                    used;
    uint32_t                tskId;
    StaticTask_t            *taskObj;
    TaskHandle_t            taskHndl;
    TaskP_mainFunction_t    taskfxn;
    void                    *arg0;
    void                    *arg1;
    bool                    terminated;
} TaskP_freertos;

/* task obj is too large to keep in structure. So keep it in global array. */
static StaticTask_t   gOsalFreertosTaskObjPool[OSAL_FREERTOS_CONFIGNUM_TASK];
/* global pool of statically allocated task pools */
static TaskP_freertos gOsalTaskPfreertosPool[OSAL_FREERTOS_CONFIGNUM_TASK];

uint32_t  gOsalTaskAllocCnt, gOsalTaskPeak;

void TaskP_compileTime_SizeChk(void)
{
#if defined(__GNUC__) && !defined(__ti__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#else
/* TI compiler */
#pragma diag_suppress 179
#endif
    OSAL_COMPILE_TIME_SIZE_CHECK ((uint32_t)sizeof(TaskP_freertos),OSAL_FREERTOS_TASKP_SIZE_BYTES);
#if defined(__GNUC__) && !defined(__ti__)
#pragma GCC diagnostic pop
#endif
}

void TaskP_Function (void *arg)
{
    TaskP_freertos *handle = (TaskP_freertos *)(arg);

    /* Call the application function. */
    (*handle->taskfxn)(handle->arg0, handle->arg1);
    
    /* Task Fxn completed execution. */
    handle->terminated = TRUE;
    /* Put vTaskSuspend in a loop just in case some calls vTaskResume, it will go back to suspend. */
    while (handle->terminated)
    {
        vTaskSuspend(NULL);
    }
    
}

/*
 *  ======== TaskP_create ========
 */
TaskP_Handle TaskP_create(void *taskfxn, const TaskP_Params *params)
{
    TaskP_Handle    ret_handle;
    TaskP_freertos *handle = (TaskP_freertos *) NULL_PTR;
    TaskP_freertos *taskPool;
    uint32_t        i;
    uintptr_t       key;
    uint32_t        maxTasks;
    uint32_t        taskPriority;

    DebugP_assert((taskfxn != NULL_PTR));
    DebugP_assert((params != NULL_PTR));
    DebugP_assert((params->stack != NULL_PTR));

    /* Pick up the internal static memory block */
    taskPool        = (TaskP_freertos *) &gOsalTaskPfreertosPool[0];
    maxTasks        = OSAL_FREERTOS_CONFIGNUM_TASK;
    
    if(gOsalTaskAllocCnt==0U) 
    {
        (void)memset( (void *)gOsalTaskPfreertosPool,0,sizeof(gOsalTaskPfreertosPool));
        (void)memset( (void *)gOsalFreertosTaskObjPool,0,sizeof(gOsalFreertosTaskObjPool));
    }

    key = HwiP_disable();

     for (i = 0; i < maxTasks; i++)
     {
         if (taskPool[i].used == FALSE)
         {
             taskPool[i].used = TRUE;
             taskPool[i].taskObj = &gOsalFreertosTaskObjPool[i];
             /* Update statistics */
             gOsalTaskAllocCnt++;
             if (gOsalTaskAllocCnt > gOsalTaskPeak)
             {
                 gOsalTaskPeak = gOsalTaskAllocCnt;
             }
             break;
         }
     }
    HwiP_restore(key);

    if (i < maxTasks)
    {
        /* Grab the memory */
        handle = (TaskP_freertos *) &taskPool[i];
        handle->tskId = i;
    }

    if (handle == NULL_PTR) {
        ret_handle = NULL_PTR;
    }
    else
    {
        taskPriority = params->priority;
        /* if prority is out of range, adjust to bring it in range */
        if(taskPriority > TaskP_PRIORITY_HIGHEST)
        {
            taskPriority = TaskP_PRIORITY_HIGHEST;
        }
        if(taskPriority <= TaskP_PRIORITY_LOWEST)
        {
            taskPriority = TaskP_PRIORITY_LOWEST;
        }
        /* Store the task function and args in taskP object. */
        handle->taskfxn = (TaskP_mainFunction_t)(taskfxn);
        handle->arg0 = params->arg0;
        handle->arg1 = params->arg1;
        handle->terminated = FALSE;

        handle->taskHndl = xTaskCreateStatic((TaskFunction_t)TaskP_Function, /* Pointer to the function that implements the task. */
                                      (char *)params->name,              /* Text name for the task.  This is to facilitate debugging only. */
                                      params->stacksize/(sizeof(UBaseType_t)),  /* Stack depth in units of StackType_t typically uint32_t on 32b CPUs */
                                      handle,       /* task specific args */
                                      taskPriority,   /* task priority, 0 is lowest priority, configMAX_PRIORITIES-1 is highest */
                                      (StackType_t*)params->stack,      /* pointer to stack base */
                                      handle->taskObj); /* pointer to statically allocated task object memory */
        if(handle->taskHndl == NULL)
        {
            /* If there was an error reset the task object and return NULL. */
            key = HwiP_disable();
            handle->used = FALSE;
            /* Found the osal task object to delete */
            if (gOsalTaskAllocCnt > 0U)
            {
                gOsalTaskAllocCnt--;
            }
            HwiP_restore(key);
            ret_handle = NULL_PTR;
        }
        else
        {
            LoadP_addTask((TaskP_Handle)handle, handle->tskId);
            ret_handle = ((TaskP_Handle)handle);
        }
    }

    return (ret_handle);
}

/*
 *  ======== TaskP_delete ========
 */
TaskP_Status TaskP_delete(TaskP_Handle *hTaskPtr)
{
    uintptr_t   key;
    TaskP_Status ret = TaskP_OK;
    TaskP_Handle hTask = *hTaskPtr;
    TaskP_freertos *task = (TaskP_freertos *)hTask;
    TaskHandle_t currentTaskHndl;

    if((task != NULL_PTR) && (task->used==TRUE))
    {
        currentTaskHndl = xTaskGetCurrentTaskHandle();
        if(currentTaskHndl == task->taskHndl)
        {
            /* This is task deleting itself. */
            DebugP_log0("Warning: vTaskDelete will not return when the task is deleting itself.\n"
                        "Resource freeing should be handled in deleteHookcallback which is not done currently\n"
                        "So there will be resource leak\n");
        }

        vTaskDelete(task->taskHndl);
        LoadP_removeTask(task->tskId);

        key = HwiP_disable();
        /* In FreeRTOS, task is deleted in the idle task.
         * So the TaskObj should not be memset to 0 */
        /* (void )memset( (void *)task->taskObj, 0, sizeof(task->taskObj)); */
        task->used      = FALSE;
        task->taskObj   = NULL;
        task->taskHndl  = NULL;
        task->taskfxn   = NULL;
        task->arg0      = NULL;
        task->arg1      = NULL;

        /* Found the osal task object to delete */
        if (gOsalTaskAllocCnt > 0U)
        {
            gOsalTaskAllocCnt--;
        }
        HwiP_restore(key);

        ret = TaskP_OK;
    }
    else
    {
       ret = TaskP_FAILURE;
    }  
    return (ret);
}

/*
 *  ======== TaskP_Params_init ========
 */
void TaskP_Params_init(TaskP_Params *params)
{
    if(params != NULL_PTR)
    {
        params->name = (uint8_t *)"FREERTOS_TASK";
        params->stacksize = 0;
        params->stack = NULL;
        params->priority = (TaskP_PRIORITY_HIGHEST - TaskP_PRIORITY_LOWEST) / 2;
        params->arg0 = NULL;
        params->arg1 = NULL;
    }
}

void TaskP_sleep(uint32_t timeout)
{
    vTaskDelay(timeout);
}

void TaskP_sleepInMsecs(uint32_t timeoutInMsecs)
{
    uint32_t ticks;

    /* portTICK_PERIOD_MS is in units of msecs */
    ticks = timeoutInMsecs / portTICK_PERIOD_MS;

    vTaskDelay(ticks);
}

void TaskP_setPrio(TaskP_Handle handle, uint32_t priority)
{
    TaskP_freertos *taskHandle = (TaskP_freertos *)handle;

    DebugP_assert((handle != NULL_PTR));
    vTaskPrioritySet(taskHandle->taskHndl, priority);
}

TaskP_Handle TaskP_self(void)
{
    TaskHandle_t taskHndl;
    TaskP_Handle retHandle = NULL_PTR;
    uint32_t        i, maxTasks;

    taskHndl = xTaskGetCurrentTaskHandle();
    if (taskHndl != NULL_PTR)
    {
        /* Now get the corresponding TaskP Handle */
        maxTasks        = OSAL_FREERTOS_CONFIGNUM_TASK;
        for (i = 0; i < maxTasks; i++)
        {
            if ((gOsalTaskPfreertosPool[i].used == TRUE) &&
                (gOsalTaskPfreertosPool[i].taskHndl == taskHndl))
            {
                retHandle = (TaskP_Handle) (&gOsalTaskPfreertosPool[i]);
                break;
            }
        }
    }

    return retHandle;
}

TaskP_Handle TaskP_selfmacro(void)
{
    /* For freertos task self is not implemented as inline macro.
     * So call the TaskP_self API itself.
     */
    return TaskP_self();
}

void TaskP_yield(void) {
    taskYIELD();
}

uint32_t TaskP_isTerminated(TaskP_Handle handle)
{
    uint32_t isTaskTerminated = 0;
    TaskP_freertos *taskHandle = (TaskP_freertos *)handle;

    DebugP_assert((handle != NULL_PTR));
    if((TRUE == taskHandle->terminated) || (eTaskGetState(taskHandle->taskHndl) == eDeleted))
    {
        isTaskTerminated = 1;
    }
    else
    {
        isTaskTerminated = 0;
    }
    return isTaskTerminated;
}

uint32_t TaskP_disable(void)
{
    vTaskSuspendAll();

    return 0;
}

void TaskP_restore(uint32_t key)
{
    xTaskResumeAll();
}

TaskHandle_t TaskP_getFreertosHandle(TaskP_Handle handle)
{
    TaskP_freertos *taskHandle = (TaskP_freertos *)handle;

    DebugP_assert((handle != NULL_PTR));
    DebugP_assert((taskHandle->used != FALSE));

    return (taskHandle->taskHndl);
}

uint32_t TaskP_getTaskId(TaskP_Handle handle)
{
    TaskP_freertos *taskHandle = (TaskP_freertos *)handle;

    DebugP_assert((handle != NULL_PTR));
    DebugP_assert((taskHandle->used != FALSE));

    return (taskHandle->tskId);
}

void OS_init( void )
{
#if defined (SOC_AWR294X)
    /* For AWR294x the soc frequency needs to be set at runtime,
     * based on package Type. Hence adding soc specific code here.
     * Update the CPU Frequency in Hw attr structure. */
    Osal_HwAttrs hwAttrs;
    uint32_t ctrlBitMap = 0;

    hwAttrs.cpuFreqKHz = CSL_SocGetCpuFreq() / 1000U;
    ctrlBitMap |= OSAL_HWATTR_SET_CPU_FREQ;

    Osal_setHwAttrs(ctrlBitMap, &hwAttrs);
#endif
}

void OS_start(void)
{
    vTaskStartScheduler();
}

void OS_stop(void)
{
    vTaskEndScheduler();
}

/* Nothing past this point */
