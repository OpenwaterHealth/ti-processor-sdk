/*
 * Support for the TDA4 GMSL board
 * Copyright (c) 2020, D3 Engineering.  All rights reserved.
 */


#ifndef _D3_GMSL_BOARD_H_
#define _D3_GMSL_BOARD_H_

#include <stdint.h>

#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

#define D3_GMSL_BOARD_I2C_BUS_ID 6

extern uint8_t d3_gmsl_board_get_i2c_bus_id(uint8_t ch_id);
extern int32_t d3_gmsl_poc_power(void *i2c_handle, uint8_t channel_mask);
extern int32_t d3_gmsl_des_pwdn(void *i2c_handle, uint32_t channel_mask);
extern int8_t d3_gmsl_board_get_des_addr(uint8_t ch_id);
extern int32_t d3_gmsl_board_disable_all_other_channels(void *i2c_handle, uint8_t ch_id);
extern int32_t d3_gmsl_board_enable_all_other_channels(void *i2c_handle, uint8_t ch_id);

#endif /* _D3_GMSL_BOARD_H_ */
