#include "gpio_utils.h"

/* GPIO Driver board specific pin configuration structure */
GPIO_PinConfig gpioPinConfigs[] =
{
    /* Frame sync Output pin */
    GPIO_DEVICE_CONFIG(GPIO_FRAMESYNC_PORT_NUM, GPIO_FRAMESYNC_PIN_NUM) |
    GPIO_CFG_OUTPUT
};

/* GPIO Driver call back functions */
GPIO_CallbackFxn gpioCallbackFunctions[] =
{
    NULL,
    NULL
};

/* GPIO Driver configuration structure */
GPIO_v0_Config GPIO_v0_config =
{
    gpioPinConfigs,
    gpioCallbackFunctions,
    sizeof(gpioPinConfigs) / sizeof(GPIO_PinConfig),
    sizeof(gpioCallbackFunctions) / sizeof(GPIO_CallbackFxn),
    0x20U
};

static int32_t pcal9539_regWrite(void *i2cHandle, uint8_t i2cAddr,
                                    uint8_t regAddr, uint8_t * regVal)
{
    return Board_i2c8BitRegWr(i2cHandle, i2cAddr, regAddr, regVal, 1, I2C_WAIT_FOREVER);
}

static int32_t pcal9539_regRead(void *i2cHandle, uint8_t i2cAddr, uint8_t regAddr, uint8_t * regVal)
{
    return Board_i2c8BitRegRd(i2cHandle, i2cAddr, regAddr, regVal, 1, I2C_WAIT_FOREVER);
}

static int32_t pcal9539_regSetBits(void *i2cHandle, uint8_t i2cAddr, uint8_t regAddr, uint8_t bits, uint8_t mask)
{
    int32_t status;
    uint8_t regVal;

    status = pcal9539_regRead(i2cHandle, i2cAddr, regAddr, &regVal);
    if (status != 0) {
        return status;
    }
    regVal &= ~mask;
    regVal |= bits & mask;
    return pcal9539_regWrite(i2cHandle, i2cAddr, regAddr, &regVal);
}

int32_t pcal9539_setDirection(void *i2cHandle, uint8_t i2cAddr,
                                uint8_t port, uint8_t bits, uint8_t mask)
{
    uint8_t regAddr;

    if (port == 0)
        regAddr = PCAL9539_REG_CONFIGURATION_PORT_0;
    else
        regAddr = PCAL9539_REG_CONFIGURATION_PORT_1;
    return pcal9539_regSetBits(i2cHandle, i2cAddr, regAddr, bits, mask);
}

int32_t pcal9539_setPolarity(void *i2cHandle, uint8_t i2cAddr,
                                uint8_t port, uint8_t bits, uint8_t mask)
{
    uint8_t regAddr;

    if (port == 0)
        regAddr = PCAL9539_REG_POLARITY_INVERSION_PORT_0;
    else
        regAddr = PCAL9539_REG_POLARITY_INVERSION_PORT_1;
    return pcal9539_regSetBits(i2cHandle, i2cAddr, regAddr, bits, mask);
}

int32_t pcal9539_setOutValue(void *i2cHandle, uint8_t i2cAddr,
                                uint8_t port, uint8_t bits, uint8_t mask)
{
    uint8_t regAddr;

    if (port == 0)
        regAddr = PCAL9539_REG_OUTPUT_PORT_0;
    else
        regAddr = PCAL9539_REG_OUTPUT_PORT_1;
    return pcal9539_regSetBits(i2cHandle, i2cAddr, regAddr, bits, mask);

}

int32_t pcal9539_getInValue(void *i2cHandle, uint8_t i2cAddr,
                                uint8_t port, uint8_t *regVal)
{
    uint8_t regAddr;
    uint8_t tmpRegVal;
    uint32_t status;

    if (port == 0)
        regAddr = PCAL9539_REG_INPUT_PORT_0;
    else
        regAddr = PCAL9539_REG_INPUT_PORT_1;
    status = pcal9539_regRead(i2cHandle, i2cAddr, regAddr, &tmpRegVal);
    /* Update regVal ONLY if the read was successful. */
    if (status == 0)
        *regVal = tmpRegVal;
    return status;
}
