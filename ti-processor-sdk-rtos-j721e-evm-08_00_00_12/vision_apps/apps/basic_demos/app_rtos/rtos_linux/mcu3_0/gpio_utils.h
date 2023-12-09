#ifndef _GPIO_UTILS_H_
#define _GPIO_UTILS_H_

#include <ti/drv/gpio/GPIO.h>
#include <ti/drv/gpio/soc/GPIO_soc.h>
#include <ti/board/src/devices/common/common.h>

#define MAIN_I2C0 0 /* i2C that controls the buff_out1 GPIO */
#define EXP8_I2C_ADDR 0x74
#define GPIO_BUFF_OUT1_PORT_NUM 1
#define GPIO_BUFF_OUT1_PIN_NUM 2

#define GPIO_FRAMESYNC_PORT_NUM 0
#define GPIO_FRAMESYNC_PIN_NUM 75

#define GPIO_FRAMESYNC_IDX 0

/* Mask for GPIO pin number */
#define GPIO_CFG_PIN_NUM_MASK     (0x000000FFU)

/* Shift for GPIO pin number */
#define GPIO_CFG_PIN_NUM_SHIFT    (0x00U)

/* Mask for GPIO port number */
#define GPIO_CFG_PORT_NUM_MASK    (0x0000FF00U)

/* Shift for GPIO port number */
#define GPIO_CFG_PORT_NUM_SHIFT   (0x8U)

/* Macro to formulated device specific port pin configuration */
#define GPIO_DEVICE_CONFIG(port, pin)  \
              ((((port) << GPIO_CFG_PORT_NUM_SHIFT) &(GPIO_CFG_PORT_NUM_MASK)) | \
              (((pin) << GPIO_CFG_PIN_NUM_SHIFT) & (GPIO_CFG_PIN_NUM_MASK)))

enum {
    PCAL9539_PORT_0 = 0,
    PCAL9539_PORT_1
};

enum {
    PCAL9539_BIT_0 = 0,
    PCAL9539_BIT_1,
    PCAL9539_BIT_2,
    PCAL9539_BIT_3,
    PCAL9539_BIT_4,
    PCAL9539_BIT_5,
    PCAL9539_BIT_6,
    PCAL9539_BIT_7,
};

enum {
    PCAL9539_REG_INPUT_PORT_0 = 0,
    PCAL9539_REG_INPUT_PORT_1,
    PCAL9539_REG_OUTPUT_PORT_0,
    PCAL9539_REG_OUTPUT_PORT_1,
    PCAL9539_REG_POLARITY_INVERSION_PORT_0,
    PCAL9539_REG_POLARITY_INVERSION_PORT_1,
    PCAL9539_REG_CONFIGURATION_PORT_0,
    PCAL9539_REG_CONFIGURATION_PORT_1
};

enum {
    PCAL9539_POLARITY_NORMAL = 0,
    PCAL9539_POLARITY_INVERTED
};

enum {
    PCAL9539_DIRECTION_OUTPUT = 0,
    PCAL9539_DIRECTION_INPUT
};

enum {
    PCAL9593_LEVEL_LOW = 0,
    PCAL9593_LEVEL_HIGH
};

int32_t pcal9539_setDirection(void *i2cHandle, uint8_t i2cAddr,
                                        uint8_t port, uint8_t bits, uint8_t mask);
int32_t pcal9539_setPolarity(void *i2cHandle, uint8_t i2cAddr,
                                        uint8_t port, uint8_t bits, uint8_t mask);
int32_t pcal9539_setOutValue(void *i2cHandle, uint8_t i2cAddr,
                                        uint8_t port, uint8_t bits, uint8_t mask);
int32_t pcal9539_getInValue(void *i2cHandle, uint8_t i2cAddr,
                                        uint8_t port, uint8_t *value);

#endif
