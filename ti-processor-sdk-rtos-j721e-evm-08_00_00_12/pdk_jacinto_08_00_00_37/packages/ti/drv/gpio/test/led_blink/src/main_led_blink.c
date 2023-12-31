/**
 *  \file   main_led_blink.c
 *
 *  \brief  Example application main file. This application will toggle the led.
 *          The led toggling will be done inside an callback function, which
 *          will be called by Interrupt Service Routine. Interrupts are
 *          triggered manually and no external source is used to trigger
 *          interrupts.
 *
 */

/*
 * Copyright (C) 2014 - 2020 Texas Instruments Incorporated - http://www.ti.com/
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the
 * distribution.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of
 * its contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <ti/csl/arch/csl_arch.h>

#include <ti/osal/osal.h>
#include <ti/osal/TaskP.h>

#include <stdio.h>
#include <string.h>

/* TI-RTOS Header files */
#include <ti/drv/gpio/GPIO.h>
#include <ti/drv/gpio/soc/GPIO_soc.h>

#include "GPIO_log.h"
#include "GPIO_board.h"

#include <ti/board/board.h>

#if defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM64X)
#include <ti/csl/soc.h>
#if defined (BUILD_DSP_1) || defined (BUILD_DSP_2)
#include  "ti/csl/csl_chipAux.h"
#endif
#endif

#if defined(SOC_AM65XX) || defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM64X)
#include <ti/drv/sciclient/sciclient.h>
#endif

#if defined (SOC_AM65XX) || defined (SOC_J721E) || defined (SOC_J7200)
#include <ti/csl/csl_clec.h>
#endif

#if defined (__C7100__)
#include <ti/csl/arch/csl_arch.h>
#endif

/**********************************************************************
 ************************** Macros ************************************
 **********************************************************************/
#if defined (SIM_BUILD)
/* TODO: To Test on EVM Update the delay macro. */
#define DELAY_VALUE       (1)
#else
#define DELAY_VALUE       (500U)   /* 500 msec */
#endif

#define APP_TSK_STACK_MAIN              (0x8000U)

#if defined (USE_BIOS) || defined (FREERTOS)
#if defined (SOC_J721E) || defined(SOC_J7200) || defined (SOC_TPR12) || defined (SOC_AWR294X) || defined(SOC_AM64X) || defined(SOC_AM65XX)
static uint8_t  gAppTskStackMain[APP_TSK_STACK_MAIN] __attribute__((aligned(32)));
#endif
#endif

/**********************************************************************
 ************************** Internal functions ************************
 **********************************************************************/
/* Delay function */
void AppDelay(unsigned int delayVal);

/* Callback function */
void AppGpioCallbackFxn(void);

void Gpio_appC7xPreInit(void);

#if defined(idkAM574x) || defined(idkAM572x) || defined(idkAM571x)
/* GPIO clock and pinmux configurations */
extern void AppGPIOInit(void);
#endif

#if defined(idkAM574x) || defined(idkAM572x)
extern void GPIOApp_UpdateBoardInfo(void);
extern void GPIOAppUpdateConfig(uint32_t *gpioBaseAddr, uint32_t *gpioPin);
#endif

#if defined(SOC_AM65XX) || defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM64X)
/* Main domain GPIO interrupt events */
#define MAIN_GPIO_INTRTR_GPIO0_BANK0_INT (0x000000C0) /* GPIO port 0 bank 0 interrupt event #, input to MAIN_GPIO_INTRTR */
#define MAIN_GPIO_INTRTR_GPIO1_BANK0_INT (0x000000C8) /* GPIO port 1 bank 0 interrupt event #, input to MAIN_GPIO_INTRTR */

/* Main domain GPIO interrupt events */
#define WKUP_GPIO_INTRTR_GPIO0_BANK0_INT (0x0000003C) /* GPIO port 0 bank 0 interrupt event #, input to WKUP_GPIO_INTRTR */


/* Main to MCU GPIO interrupt router mux output events */
#define CSL_MAIN2MCU_INTRTR_PLS_GPIOMUX_INT0_DFLT_PLS  (0x00000000)
#define CSL_MAIN2MCU_INTRTR_PLS_GPIOMUX_INT31_DFLT_PLS (0x0000001F)

void GPIO_configIntRouter(uint32_t portNum, uint32_t pinNum, uint32_t gpioIntRtrOutIntNum, GPIO_v0_HwAttrs *cfg)
{
    GPIO_IntCfg       *intCfg;
    uint32_t           bankNum = 0U;

    intCfg = cfg->intCfg;

#if defined (am65xx_evm) || defined (am65xx_idk)

    struct tisci_msg_rm_get_resource_range_resp res = {0};
    struct tisci_msg_rm_get_resource_range_req  req = {0};
    int32_t retVal = 0;
    uint16_t intNum, dst_id = TISCI_DEV_GIC0;

    {
        #if defined(BUILD_MCU)
        CSL_ArmR5CPUInfo r5CpuInfo;
        CSL_armR5GetCpuID(&r5CpuInfo);
        if (r5CpuInfo.grpId == (uint32_t)CSL_ARM_R5_CLUSTER_GROUP_ID_0)
        {
            if(r5CpuInfo.cpuID == 0U)
            {
                dst_id = TISCI_DEV_MCU_ARMSS0_CPU0;
            }
            else
            {
                dst_id = TISCI_DEV_MCU_ARMSS0_CPU1;
            }
        }
        #endif /* defined(BUILD_MCU) */
    }

    req.type = TISCI_DEV_WKUP_GPIOMUX_INTRTR0;
    req.secondary_host = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;
    retVal = Sciclient_rmGetResourceRange(&req,
                                        &res,
                                        SCICLIENT_SERVICE_WAIT_FOREVER);

    if (CSL_PASS != retVal || res.range_num == 0)
    {
        UART_printf("\n GPIO LED Blink App - Error \n");
        UART_printf("\n Could not register interrupts \n");
    }
    else
    {
        retVal = Sciclient_rmIrqTranslateIrOutput(req.type,
                                        res.range_start,
                                        dst_id,
                                        &intNum);
    /* no main domain GPIO pins directly connected to LEDs on GP EVM,
       use WKUP domain GPIO pins which connected to LEDs on base board */
    cfg->baseAddr = CSL_WKUP_GPIO0_BASE;

    bankNum = pinNum/16; /* Each GPIO bank has 16 pins */

    /* WKUP GPIO int router input interrupt is the GPIO bank interrupt */
    intCfg[pinNum].intNum = intNum + bankNum;
    intCfg[pinNum].intNum = intNum + bankNum;

    intCfg[pinNum].eventId = 0;
    intCfg[pinNum].intcMuxNum = INVALID_INTC_MUX_NUM;
    intCfg[pinNum].intcMuxInEvent = 0;
    intCfg[pinNum].intcMuxOutEvent = 0;

    }
#else
    /* Use main domain GPIO pins directly connected to IDK EVM */

    bankNum = pinNum/16; /* Each GPIO bank has 16 pins */
    if (portNum == 0)
    {
        /* MAIN GPIO int router input interrupt is the GPIO bank interrupt */
    #if defined (__aarch64__)
        #if defined (SOC_AM65XX)
            intCfg[pinNum].intNum = CSL_GIC0_INTR_MAIN_GPIOMUX_INTROUTER_MAIN_GPIOMUX_INTROUTER_MAIN_0_BUS_OUTP_0 + bankNum;
        #endif
    #else
        #if defined (SOC_AM65XX)
            intCfg[pinNum].intNum = CSL_MCU0_INTR_MAIN2MCU_PULSE_INTR0_OUTP_0 + bankNum;
        #endif
    #endif
    }
    else
    {
    #if defined (__aarch64__)
        #if defined (SOC_AM65XX)
            intCfg[pinNum].intNum = CSL_GIC0_INTR_MAIN_GPIOMUX_INTROUTER_MAIN_GPIOMUX_INTROUTER_MAIN_0_BUS_OUTP_6 + bankNum;
        #endif
    #else
        #if defined (SOC_AM65XX)
            intCfg[pinNum].intNum = CSL_MCU0_INTR_MAIN2MCU_PULSE_INTR0_OUTP_6 + bankNum;
        #endif
    #endif
    }
    intCfg[pinNum].eventId = 0;
    intCfg[pinNum].intcMuxNum = INVALID_INTC_MUX_NUM;
    intCfg[pinNum].intcMuxInEvent = 0;
    intCfg[pinNum].intcMuxOutEvent = 0;

#endif

#if defined(j721e_sim) || defined (j721e_evm) || defined (j7200_evm)

    /* no main domain GPIO pins directly connected to LEDs on GP EVM,
       use WKUP domain GPIO pins which connected to LEDs on base board */
    cfg->baseAddr = CSL_WKUP_GPIO0_BASE;

    bankNum = pinNum/16; /* Each GPIO bank has 16 pins */

    /* WKUP GPIO int router input interrupt is the GPIO bank interrupt */
    #if defined (BUILD_MPU)
        intCfg[pinNum].intNum = CSLR_COMPUTE_CLUSTER0_GIC500SS_SPI_WKUP_GPIOMUX_INTRTR0_OUTP_16 + bankNum;
    #endif
    #if defined (BUILD_C7X_1)
        intCfg[pinNum].eventId = CSLR_COMPUTE_CLUSTER0_GIC500SS_SPI_WKUP_GPIOMUX_INTRTR0_OUTP_16 + 992 + bankNum;	/* GPIO_CLEC_GIC_SPI_IN_EVT_OFFSET is 992 */
    #endif
    #if defined (BUILD_MCU)
        intCfg[pinNum].intNum = CSLR_MCU_R5FSS0_CORE0_INTR_WKUP_GPIOMUX_INTRTR0_OUTP_0 + bankNum;
    #endif
    intCfg[pinNum].intcMuxNum = INVALID_INTC_MUX_NUM;
    intCfg[pinNum].intcMuxInEvent = 0;
    intCfg[pinNum].intcMuxOutEvent = 0;
#endif

#if defined(am64x_evm)

    cfg->baseAddr = CSL_GPIO0_BASE;

    bankNum = pinNum/16; /* Each GPIO bank has 16 pins */

    /* Main GPIO int router input interrupt is the GPIO bank interrupt */
    #if defined (BUILD_MPU)
        intCfg[pinNum].intNum = CSLR_GICSS0_SPI_MAIN_GPIOMUX_INTROUTER0_OUTP_0 + bankNum;
    #endif
    #if defined (BUILD_MCU)
        intCfg[pinNum].intNum = CSLR_R5FSS0_CORE0_INTR_MAIN_GPIOMUX_INTROUTER0_OUTP_0 + bankNum;
    #endif
    intCfg[pinNum].intcMuxNum = INVALID_INTC_MUX_NUM;
    intCfg[pinNum].intcMuxInEvent = 0;
    intCfg[pinNum].intcMuxOutEvent = 0;

#endif

    GPIO_log("\nIntConfig:  portNum[%d], pinNum[%d], bankNum[%d], intNum[%d], eventId[%d]", portNum, pinNum,bankNum, intCfg[pinNum].intNum, intCfg[pinNum].eventId);
}
#endif /* #if defined(SOC_AM65XX) || defined(SOC_J721E) || defined(SOC_AM64X) */

/*
 *  ======== Board_initI2C ========
 */
static void Board_initGPIO(void)
{
    Board_initCfg boardCfg;

#if defined(SOC_K2H) || defined(SOC_K2K) || defined(SOC_K2E) || defined(SOC_K2L) || defined(SOC_K2G) || defined(SOC_C6678) || defined(SOC_C6657) || defined(SOC_OMAPL137) || defined(SOC_OMAPL138) || defined(SOC_AM65XX) || defined(SOC_J721E) || defined(SOC_J7200) || defined(SOC_AM64X)
    GPIO_v0_HwAttrs gpio_cfg;

    /* Get the default SPI init configurations */
    GPIO_socGetInitCfg(GPIO_LED0_PORT_NUM, &gpio_cfg);

#if defined(SOC_K2G)
    /* Setup GPIO interrupt configurations */
    GPIO_socSetIntMux(GPIO_LED0_PORT_NUM, GPIO_LED0_PIN_NUM, NULL, GPIO_MUX_SEL);
#endif
#if defined(SOC_OMAPL137) || defined(SOC_OMAPL138)
    /* Setup GPIO interrupt configurations */
    GPIO_socSetBankInt(GPIO_LED0_PORT_NUM, GPIO_LED0_PIN_NUM, NULL);
#endif
#endif

#if defined(evmK2E) || defined(evmC6678)
    boardCfg = BOARD_INIT_MODULE_CLOCK |
        BOARD_INIT_UART_STDIO;
#else
    boardCfg = BOARD_INIT_PINMUX_CONFIG |
        BOARD_INIT_MODULE_CLOCK |
        BOARD_INIT_UART_STDIO;
#endif
    Board_init(boardCfg);

#if defined(idkAM572x) || defined(idkAM574x)
    GPIOApp_UpdateBoardInfo();
#endif

    /* Modify the default GPIO configurations if necessary */
#if defined (am65xx_evm) || defined (am65xx_idk)

    GPIO_configIntRouter(GPIO_LED0_PORT_NUM, GPIO_LED0_PIN_NUM, 0, &gpio_cfg);

    /* Set the default GPIO init configurations */
    GPIO_socSetInitCfg(GPIO_LED0_PORT_NUM, &gpio_cfg);

#endif

    /* Modify the default GPIO configurations if necessary */
#if defined (j721e_evm)  || defined (j721e_sim) || defined (j7200_evm)

	/* change default GPIO port from MAIN GPIO0 to WAKEUP GPIO0 to access TP45 */
    gpio_cfg.baseAddr = CSL_WKUP_GPIO0_BASE;
#if defined (BUILD_MPU)
	gpio_cfg.intCfg->intNum = CSLR_COMPUTE_CLUSTER0_GIC500SS_SPI_WKUP_GPIOMUX_INTRTR0_OUTP_16;
#endif
#if defined (BUILD_MCU)
	gpio_cfg.intCfg->intNum = CSLR_MCU_R5FSS0_CORE0_INTR_WKUP_GPIOMUX_INTRTR0_OUTP_0;
#endif
#if defined (BUILD_C7X_1)
	gpio_cfg.intCfg->eventId = CSLR_COMPUTE_CLUSTER0_GIC500SS_SPI_WKUP_GPIOMUX_INTRTR0_OUTP_16 + 992;
#endif

/* --- TODO: move this into the board library --- */
/* For SYSBIOS only */
#ifndef BAREMETAL
#if defined (SOC_J721E)
/* set up C7x CLEC for DMTimer0 */
#if defined (BUILD_C7X_1)
    CSL_ClecEventConfig   cfgClec;
    CSL_CLEC_EVTRegs     *clecBaseAddr = (CSL_CLEC_EVTRegs *)CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE;
    uint32_t input         = CSLR_COMPUTE_CLUSTER0_GIC500SS_SPI_TIMER0_INTR_PEND_0 + 992; /* Used for Timer Interrupt */

    /* Configure CLEC for DMTimer0, SYS/BIOS uses interrupt 14 for DMTimer0 by default */
    cfgClec.secureClaimEnable = FALSE;
    cfgClec.evtSendEnable     = TRUE;
    cfgClec.rtMap             = CSL_CLEC_RTMAP_CPU_ALL;
    cfgClec.extEvtNum         = 0;
    cfgClec.c7xEvtNum         = 14;
    CSL_clecConfigEvent(clecBaseAddr, input, &cfgClec);
#endif /* for C7X cores */

/* set up C66x Interrupt Router for DMTimer0 for C66x */
#if defined (BUILD_DSP_1) || defined (BUILD_DSP_2)
    int32_t                              retVal;
    struct tisci_msg_rm_irq_set_req      rmIrqReq;
    struct tisci_msg_rm_irq_set_resp     rmIrqResp;
    uint16_t                             dst_id;
    uint16_t                             dst_host_irq;

    /* Set up C66x interrupt router for DMTimer0 */
    memset (&rmIrqReq, 0, sizeof(rmIrqReq));
    rmIrqReq.secondary_host = TISCI_MSG_VALUE_RM_UNUSED_SECONDARY_HOST;
    rmIrqReq.src_id = TISCI_DEV_TIMER0;
    rmIrqReq.src_index = 0; /* set to 0 for non-event based interrupt */

    /* Set the destination interrupt */
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_DST_ID_VALID;
    rmIrqReq.valid_params |= TISCI_MSG_VALUE_RM_DST_HOST_IRQ_VALID;

    if (CSL_chipReadDNUM() == 0U)
    {
        /* Set the destination for core0 */
       dst_id = TISCI_DEV_C66SS0_CORE0;
       /* rmIrqReq.dst_host_irq has to match the DMTimer.timerSettings[0].eventId defined in sysbios_c66.cfg */
       dst_host_irq = 21U;
    }
    else
    {
        /* Set the destination for core1 */
       dst_id = TISCI_DEV_C66SS1_CORE0;
       dst_host_irq = 20U;
    }
    rmIrqReq.dst_id       = dst_id;
    rmIrqReq.dst_host_irq = dst_host_irq; /* DMSC dest event, input to C66x INTC  */

    /* Config event */
    retVal = Sciclient_rmIrqSet(
                (const struct tisci_msg_rm_irq_set_req *)&rmIrqReq,
                &rmIrqResp,
                SCICLIENT_SERVICE_WAIT_FOREVER);
    if(0U != retVal)
    {
       return;
    }
#endif /* for C66X cores */
#endif /* for SOC_J721E */
#endif /* for SYSBIOS */
/* --- TODO: move this into the board library --- */

    GPIO_configIntRouter(GPIO_LED0_PORT_NUM, GPIO_LED0_PIN_NUM, 0, &gpio_cfg);

    /* For J721E EVM, there is not GPIO pin directly connected to LEDs */
	/* J7ES: use WAKEUP GPIO0_6 --> TP45 for testing */
    /* Set the default GPIO init configurations */
    GPIO_socSetInitCfg(GPIO_LED0_PORT_NUM, &gpio_cfg);

#endif
#if defined (SOC_TPR12) || defined (SOC_AWR294X)
  GPIO_board_init_pinconfig();
#endif

#if defined (am64x_evm)
    GPIO_configIntRouter(GPIO_LED0_PORT_NUM, GPIO_LED0_PIN_NUM, 0, &gpio_cfg);
    GPIO_socSetInitCfg(GPIO_LED0_PORT_NUM, &gpio_cfg);
    /* set pinmux for user led0 pin */
    *((volatile uint32_t *)(0xf4004)) = 0x50007;
#endif
}

/**********************************************************************
 ************************** Global Variables **************************
 **********************************************************************/
volatile uint32_t gpio_intr_triggered = 0;
volatile uint32_t last_gpio_intr_count = 0;
uint32_t gpioBaseAddr;
uint32_t gpioPin;

/*
 *  ======== test function ========
 */
#if defined (RTOS_ENV)
void gpio_test(void* arg0, void* arg1)
{
#else
int main()
{
    Board_initGPIO();
#endif

    /* GPIO initialization */
    GPIO_init();

    /* Set the callback function */
    GPIO_setCallback(USER_LED0, AppGpioCallbackFxn);

    /* Enable GPIO interrupt on the specific gpio pin */
    GPIO_enableInt(USER_LED0);

    /* Write high to gpio pin to control LED1 */
    GPIO_write((USER_LED1), GPIO_PIN_VAL_HIGH);
    AppDelay(DELAY_VALUE);

    GPIO_log("\n GPIO Led Blink Application \n");

#if defined(SOC_K2L) || defined(SOC_C6678) || defined(SOC_C6657)
    /* No GPIO pin directly connected to user LED's on K2L/C6678/C6657/AM65xx EVM, just trigger interrupt once */
    GPIO_toggle(USER_LED0);
    while (!gpio_intr_triggered);

    UART_printStatus("\n All tests have passed \n");
#ifdef USE_BIOS
    Task_exit();
#endif

#elif defined (SOC_TPR12) || defined (SOC_AWR294X)
    {
        uint32_t loopCnt = 4;
        while (loopCnt-- > 0U)
        {
            uint32_t pinVal;
            while (!gpio_intr_triggered);
            pinVal = GPIO_read(USER_LED0);
            GPIO_log(" GPIO USER_LED0 Input pin Value: %d\n", pinVal);
            gpio_intr_triggered = 0;
            /* Toggle LED1 */
            GPIO_toggle(USER_LED1);
            AppDelay(DELAY_VALUE);
        }
        GPIO_log("\n All tests have passed \n");
    }
#else

    UART_printStatus("\n Awaiting interrupt occurrence \n");

    while(1)
    {
#if defined(SOC_AM574x) || defined(SOC_AM572x) || defined(SOC_AM571x)|| defined(SOC_AM335x) || defined(SOC_AM437x)

#if defined (idkAM572x) || defined (idkAM574x)
        /* Update GPIO info based on the board */
        GPIOAppUpdateConfig(&gpioBaseAddr, &gpioPin);
#else
        gpioBaseAddr = GPIO_BASE_ADDR;
        gpioPin      = GPIO_LED_PIN;
#endif
        /* Trigger interrupt */
        GPIOTriggerPinInt(gpioBaseAddr, 0, gpioPin);
#endif
#if defined(SOC_K2H) || defined(SOC_K2K) || defined(SOC_K2E) || defined(SOC_K2G) || defined(SOC_OMAPL137) || defined(SOC_OMAPL138) || defined(SOC_AM65XX) || defined(SOC_J721E) || defined(SOC_J7200)
        GPIO_toggle(USER_LED0);
#elif defined(SOC_AM64X)
        /* In AM64x GP EVM, USER_LED0 (TEST_LED1) is connected to IO EXPANDER.
         * So, toggling USER_LED1 (TEST_LED2) - connected to MCU_GPIO0_5(EVM-LD26) */
        GPIO_toggle(USER_LED1);
#endif
        AppDelay(DELAY_VALUE);
        if (gpio_intr_triggered != last_gpio_intr_count)
        {
            UART_printf("\n Blink Iteration - %d \n", gpio_intr_triggered);
            last_gpio_intr_count = gpio_intr_triggered;

            /* This string is required to ensure automated tests and result
                parser is able to determine status of this application */
            UART_printStatus("\n All tests have passed \n");
        }
    }
#endif
}

#if defined (RTOS_ENV)
/*
 *  ======== main ========
 */
int main(void)
{
    /* Call board init functions */
    Gpio_appC7xPreInit();
    Board_initGPIO();

#if defined(idkAM574x) || defined(idkAM572x) || defined(idkAM571x)
    AppGPIOInit();
#endif

#if defined (SOC_J721E) || defined(SOC_J7200) || defined (SOC_TPR12) || defined (SOC_AWR294X) || defined(SOC_AM64X) || defined(SOC_AM65XX)
    TaskP_Params taskParams;

    OS_init();

    TaskP_Params_init(&taskParams);
    taskParams.priority =2;
    taskParams.stack        = gAppTskStackMain;
    taskParams.stacksize    = sizeof (gAppTskStackMain);

    TaskP_create(gpio_test, &taskParams);
#endif

    OS_start();

    return (0);
}
#endif

/*
 *  ======== AppDelay ========
 */
void AppDelay(unsigned int delayVal)
{
    Osal_delay(delayVal);
}

/*
 *  ======== AppLoopDelay ========
 */
void AppLoopDelay(uint32_t delayVal)
{
    volatile uint32_t i;

    for (i = 0; i < (delayVal * 1000); i++)
        ;
}

/*
 *  ======== Callback function ========
 */
#if defined (SOC_TPR12) || defined (SOC_AWR294X)
void AppGpioCallbackFxn(void)
{
    gpio_intr_triggered = 1;
}
#else
void AppGpioCallbackFxn(void)
{
#if !defined(SOC_J721E) || !defined(SOC_J7200) || !defined(SOC_AM64X)
    /* Toggle LED1 */
    GPIO_toggle(USER_LED1);
    AppLoopDelay(DELAY_VALUE);
#endif
    gpio_intr_triggered++;
}
#endif

#if defined(BUILD_MPU) || defined (__C7100__)
extern void Osal_initMmuDefault(void);
void InitMmu(void)
{
    Osal_initMmuDefault();
}
#endif

void Gpio_appC7xPreInit(void)
{
#if defined (__C7100__)
    CSL_ClecEventConfig cfgClec;
	CSL_CLEC_EVTRegs   *clecBaseAddr = (CSL_CLEC_EVTRegs*) CSL_COMPUTE_CLUSTER0_CLEC_REGS_BASE;

    uint32_t            i, maxInputs = 2048U;

    /* make secure claim bit to FALSE so that after we switch to non-secure mode
     * we can program the CLEC MMRs
     */
    cfgClec.secureClaimEnable = FALSE;
    cfgClec.evtSendEnable     = FALSE;
    cfgClec.rtMap             = CSL_CLEC_RTMAP_DISABLE;
    cfgClec.extEvtNum         = 0U;
    cfgClec.c7xEvtNum         = 0U;
    for(i = 0U; i < maxInputs; i++)
    {
        CSL_clecConfigEvent(clecBaseAddr, i, &cfgClec);
    }
#endif

    return;
}
