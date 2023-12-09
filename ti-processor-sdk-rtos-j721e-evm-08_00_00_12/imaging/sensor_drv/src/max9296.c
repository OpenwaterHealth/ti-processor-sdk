/*
 * MAX9296 GMSL2 deserializer.
 * Copyright (c) 2020, D3 Engineering.  All rights reserved.
 */

#include <stdint.h>
#include <iss_sensors.h>
#include <iss_sensor_if.h>
#include <iss_sensor_priv.h>
#include "max9296.h"
#include "d3_gmsl_board.h"

int32_t max9296_set_csi_state(void *i2c_handle, uint32_t des_id, uint32_t gmsl_state)
{
    uint16_t regAddr = MAX9296_REG_BACKTOP12;
    uint8_t regVal;
    int32_t status;
    uint8_t i2cDesAddr = d3_gmsl_board_get_des_addr(des_id);

    status = Board_i2c16BitRegRd(i2c_handle, i2cDesAddr, regAddr, &regVal,
                                    1U, BOARD_I2C_REG_ADDR_MSB_FIRST, I2C_WAIT_FOREVER);
    if (status != 0) {
        appLogPrintf("Error reading 0x%x from 0x%x\n", regAddr, i2cDesAddr);
        return status;
    }
    if (gmsl_state)
        regVal |= MAX9296_CSI_OUT_EN;
    else
        regVal &= ~MAX9296_CSI_OUT_EN;
    return Board_i2c16BitRegWr(i2c_handle, i2cDesAddr, regAddr, &regVal,
                                    1U, BOARD_I2C_REG_ADDR_MSB_FIRST, I2C_WAIT_FOREVER);
}

int32_t max9296_enable_csi(void *i2c_handle, uint32_t des_id)
{
    return max9296_set_csi_state(i2c_handle, des_id, TRUE);
}

int32_t max9296_disable_csi(void *i2c_handle, uint32_t des_id)
{
    return max9296_set_csi_state(i2c_handle, des_id, FALSE);
}

int32_t max9296_select_gmsl_link(void *i2c_handle, uint32_t des_id, uint8_t link_mode)
{
    uint8_t des_addr = d3_gmsl_board_get_des_addr(des_id);
    uint8_t reg_val = MAX9296_RESET_ONESHOT | (link_mode & 3);
    return Board_i2c16BitRegWr(i2c_handle, des_addr, MAX9296_REG_CTRL0, &reg_val,
                                1U, BOARD_I2C_REG_ADDR_MSB_FIRST, I2C_WAIT_FOREVER);
}
