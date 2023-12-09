/*
 * Support for the MAX9295 GMSL2 serializer
 * Copyright (c) 2020, D3 Engineering.  All rights reserved.
 */


#ifndef _MAX9295_H_
#define _MAX9295_H_

#define MAX9295_REG0 0x0
#define MAX9295_REG1 0x1

#define MAX9295_REG_CTRL0 0x10

#define MAX9295_REG_SRC_A 0x42
#define MAX9295_REG_DST_A 0x43
#define MAX9295_REG_SRC_B 0x44
#define MAX9295_REG_DST_B 0x45

#define MAX9295_REG_MIPI_RX2 0x0332
#define MAX9295_REG_MIPI_RX3 0x0333

extern int32_t max9295_change_ser_addr(void *i2c_handle, uint8_t des_addr,
                                uint8_t ser_addr, uint8_t new_ser_addr);
extern int32_t max9295_change_img_addr(void *i2c_handle, uint8_t ser_addr,
                                uint8_t img_addr, uint8_t new_img_addr);
extern int32_t max9295_set_csi_lane_mapping(void *i2c_handle, uint8_t ser_addr, uint8_t *lane_map);

#endif /* _MAX9295_H_ */
