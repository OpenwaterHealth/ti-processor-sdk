/*
 * Support for the TDA4 GMSL board
 * Copyright (c) 2020, D3 Engineering.  All rights reserved.
 */

#ifndef _PCAL9539_H_
#define _PACL9539_H_

#include <stdint.h>

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

extern int32_t pcal9539_setDirection(void *i2cHandle, uint8_t i2cAddr,
                                        uint8_t port, uint8_t bits, uint8_t mask);
extern int32_t pcal9539_setPolarity(void *i2cHandle, uint8_t i2cAddr,
                                        uint8_t port, uint8_t bits, uint8_t mask);
extern int32_t pcal9539_setOutValue(void *i2cHandle, uint8_t i2cAddr,
                                        uint8_t port, uint8_t bits, uint8_t mask);
extern int32_t pcal9539_getInValue(void *i2cHandle, uint8_t i2cAddr,
                                        uint8_t port, uint8_t *value);

#endif /* _PCAL9539_H_ */
