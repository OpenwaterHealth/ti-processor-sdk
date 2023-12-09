/*
 *
 *
 */
#ifndef _HIMAX5530_SERDES_H_
#define _HIMAX5530_SERDES_H_

/**
7-bit Alias addresses for sensor and serializer
Physical addresses must be programmed in UB96x config
SoC will communicate with the devices using alias adresses
*/

#define HIMAX5530_I2C_ADDR 0x24

#define HIMAX5530_SER_CFG_SIZE    (8U)

// #define MCLK_15_MHZ (1) // Be sure to change FSIN periods in app_multi_cam_histo.cfg
#define MCLK_19_2_MHZ (1)
// #define MCLK_25_MHZ  (1)
// MCLK_19_2_MHZ_60HZ (1)
// MCLK_19_2_MHZ_80HZ (1)

I2cParams ub953SerCfg_HIMAX5530[HIMAX5530_SER_CFG_SIZE] = {
#ifdef MCLK_25_MHZ
    {0x01, 0x01, 0x20},
    {0x02, 0x72, 0x10},
    {0x06, 0x41, 0x0},
    {0x07, 0x28, 0x100}, // Configure clock to sensor.
    {0x0E, 0xF0, 0x100},
    {0x0D, 0x2C, 0x100},
    {0x0D, 0x2D, 0x10}, // Pull sensor reset lines.
    {0xFFFF, 0x00, 0x0} //End of script
#elif MCLK_19_2_MHZ || MCLK_19_2_MHZ_60HZ || MCLK_19_2_MHZ_80HZ
    {0x01, 0x01, 0x20},
    {0x02, 0x72, 0x10},
    {0x06, 0x41, 0x0},
    {0x07, 0x34, 0x100}, // Configure clock to sensor. 4000/4 = 1000, 1000/0x34 = 19.23076
    {0x0E, 0xF0, 0x100},
    {0x0D, 0x2C, 0x100},
    {0x0D, 0x2D, 0x10}, // Pull sensor reset lines.  3C for EVM camera;  0x03 for D3 953 socket + OW red camera board
    {0xFFFF, 0x00, 0x0} //End of script
#elif MCLK_15_MHZ
    {0x01, 0x01, 0x20},
    {0x02, 0x72, 0x10},
    {0x06, 0x41, 0x0},
    {0x07, 0x42, 0x100}, // Configure clock to sensor 15.152 MHz
    {0x0E, 0xF0, 0x100},
    {0x0D, 0x2C, 0x100},
    {0x0D, 0x2D, 0x10}, // Pull sensor reset lines.  3C for EVM camera;  0x03 for D3 953 socket + OW red camera board
    {0xFFFF, 0x00, 0x0} //End of script
#endif
};

#endif /* _HIMAX5530_SERDES_H_ */
