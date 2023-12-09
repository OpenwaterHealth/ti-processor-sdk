/*
 * MAX9295 GMSL2 serializer.
 * Copyright (c) 2020, D3 Engineering.  All rights reserved.
 */

#include <stdint.h>
#include <iss_sensor_priv.h>
#include "max9295.h"

int32_t max9295_change_ser_addr(void *i2c_handle, uint8_t des_addr,
                                uint8_t ser_addr, uint8_t new_ser_addr)
{
    int32_t status;
    uint8_t reg_val = new_ser_addr << 1;

    status = Board_i2c16BitRegWr(i2c_handle, ser_addr, MAX9295_REG0, &reg_val,
                                    1U, BOARD_I2C_REG_ADDR_MSB_FIRST, I2C_WAIT_FOREVER);
    if (status == 0) {
        appLogPrintf("MAX9295: address changed 0x%x => 0x%x.\n", ser_addr, new_ser_addr);
    }
    return status;
}

int32_t max9295_change_img_addr(void *i2c_handle, uint8_t ser_addr,
                                uint8_t img_addr, uint8_t new_img_addr)
{
    int32_t status = 0;
    uint8_t reg_val = new_img_addr << 1;

    status = Board_i2c16BitRegWr(i2c_handle, ser_addr, MAX9295_REG_SRC_A, &reg_val,
                                    1U, BOARD_I2C_REG_ADDR_MSB_FIRST, I2C_WAIT_FOREVER);
    reg_val = img_addr << 1;
    status |= Board_i2c16BitRegWr(i2c_handle, ser_addr, MAX9295_REG_DST_A, &reg_val,
                                    1U, BOARD_I2C_REG_ADDR_MSB_FIRST, I2C_WAIT_FOREVER);
    if (status == 0) {
        appLogPrintf("MAX9295: imager address changed: 0x%x => 0x%x.\n", img_addr, new_img_addr);
    }
    return status;
}

/* lane_map is an array of 4 numbers that are a combination of (0, 1, 2, 3), e.g. { 3, 2, 1, 0 } */
int32_t max9295_set_csi_lane_mapping(void *i2c_handle, uint8_t ser_addr, uint8_t *lane_map)
{
    int32_t status = 0;
    uint8_t reg_val = 0;

    reg_val = lane_map[1] << 2 | lane_map[0];
    status = Board_i2c16BitRegWr(i2c_handle, ser_addr, MAX9295_REG_MIPI_RX3, &reg_val,
                                    1U, BOARD_I2C_REG_ADDR_MSB_FIRST, I2C_WAIT_FOREVER);
    reg_val = (lane_map[3] << 2 | lane_map[2]) << 4;
    status = Board_i2c16BitRegWr(i2c_handle, ser_addr, MAX9295_REG_MIPI_RX3, &reg_val,
                                    1U, BOARD_I2C_REG_ADDR_MSB_FIRST, I2C_WAIT_FOREVER);
    return status;
}
