#ifndef APP_SCICLIENT_HELPER_H__
#define APP_SCICLIENT_HELPER_H__

#define SET_CLOCK_PARENT(MOD, CLK, PARENT, STATUS) do { \
    if(STATUS == CSL_PASS) { \
        STATUS = Sciclient_pmSetModuleClkParent(MOD, CLK, PARENT, SCICLIENT_SERVICE_WAIT_FOREVER); \
        if(STATUS != CSL_PASS) App_printf("Sciclient_pmSetModuleClkParent (mod = %u, clk = %u, parent = %u) failed\n", MOD, CLK, PARENT); \
    } \
} while(0)

#define SET_DEVICE_STATE(MOD, STATE, STATUS) do { \
    if(status == CSL_PASS) { \
        STATUS = Sciclient_pmSetModuleState(MOD, TISCI_MSG_VALUE_DEVICE_SW_STATE_ON, TISCI_MSG_FLAG_AOP, SCICLIENT_SERVICE_WAIT_FOREVER); \
        if(STATUS != CSL_PASS) App_printf("Sciclient_pmSetModuleState (mod = %u) failed\n", MOD); \
    } \
} while(0)

#define SET_CLOCK_STATE(MOD, CLK, FLAG, STATE, STATUS) do { \
    if(STATUS == CSL_PASS) { \
        STATUS = Sciclient_pmModuleClkRequest(MOD, CLK, STATE, FLAG, SCICLIENT_SERVICE_WAIT_FOREVER); \
        if(STATUS != CSL_PASS) App_printf("Sciclient_pmModuleClkRequest (mod = %u, clk = %u) failed\n", MOD, CLK); \
    } \
} while(0)

#define SET_CLOCK_FREQ(MOD, CLK, FREQ, STATUS) do { \
    if(STATUS == CSL_PASS) { \
        STATUS = Sciclient_pmSetModuleClkFreq(MOD, CLK, FREQ, 0, SCICLIENT_SERVICE_WAIT_FOREVER); \
        if(status != CSL_PASS) App_printf("Sciclient_pmSetModuleClkFreq (mod = %u, clk = %u, freq = %u%06u) failed\n", MOD, CLK, (uint32_t)(FREQ / 1000000), (uint32_t)(FREQ % 1000000)); \
    } \
} while(0)

#endif

