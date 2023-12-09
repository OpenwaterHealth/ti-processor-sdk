/*
 * Support for the MAX9296 GMSL2 deserializer
 * Copyright (c) 2020, D3 Engineering.  All rights reserved.
 */


#ifndef _MAX9296_H_
#define _MAX9296_H_

#define MAX9296_REG_CTRL0 0x10
#define MAX9296_REG_BACKTOP12 0x313

#define MAX9296_CSI_OUT_EN (1 << 1)

#define MAX9296_RESET_ALL (1 << 7)
#define MAX9296_RESET_LINK (1 << 6)
#define MAX9296_RESET_ONESHOT (1 << 5)
#define MAX9296_AUTO_LINK (1 << 4)

enum {
    MAX9296_LINK_MODE_AUTO = 0, /* Auto mode; any one of the channel can be used */
    MAX9296_LINK_MODE_A,        /* Only channel A can be used */
    MAX9296_LINK_MODE_B,        /* Only channel B can be used */
    MAX9296_LINK_MODE_SPLITTER  /* Both channels; but both need to be connected, otherwise none comes up */
};

extern int32_t max9296_enable_csi(void *i2c_handle, uint32_t des_id);
extern int32_t max9296_disable_csi(void *i2c_handle, uint32_t des_id);
int32_t max9296_select_gmsl_link(void *i2c_handle, uint32_t des_id, uint8_t link_mode);

#endif /* _MAX9296_H_ */
