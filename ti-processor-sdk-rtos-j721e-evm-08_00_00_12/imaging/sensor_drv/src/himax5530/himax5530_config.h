/*    Open Water Internet Inc
*/
#ifndef HIMAX5530_CONFIG_H_
#define HIMAX5530_CONFIG_H_

#define MCLK_19_2_MHZ (1)
// #define MCLK_25_MHZ  (1)
// MCLK_19_2_MHZ_60HZ (1)
// MCLK_19_2_MHZ_80HZ (1)

typedef enum himaxreg{  /* sequence matters,  not the values here, used as index*/
	//10.1
	R_MODEL_ID_H,
	R_MODEL_ID_L,
	R_SILICON_REV,
	R_FRAME_COUNT,
	R_PIXEL_ORDER,
	//10.2
	R_MODE_SELECT,
	R_IMAGE_ORIENTATION,
	R_SW_RESET,
	R_COMMAND_UPDATE,
	//10.3
	R_COARSE_INTEGRATION_H,		R_COARSE_INTEGRATION_L,
	R_ANALOG_GLOBAL_GAIN,
	R_DIGITAL_GLOBAL_GAIN_H,	R_DIGITAL_GLOBAL_GAIN_L,
	//10.4
	R_VT_SYS_CLK_DIV,
	R_ADCPLL_CFG,
	R_PRE_PLL_CLK_DIV,
	R_PULTIPLIER,
	R_OP_PIX_CLK_DIV,
	R_MIPIPLL_CFG,
	R_PRE_MIPI_PLL_CLK_DIV,
	R_PLL_MIPI_MULTIPLIER,
	//10.5
	R_FRAME_LENGTH_LINES_H,	R_FRAME_LENGTH_LINES_L,
	R_LINE_LENGTH_PCK_H,		R_LINE_LENGTH_PCK_L,
	//10.6
	R_X_START_ADDR_H, 		R_X_START_ADDR_L,
	R_Y_START_ADDR_H, 		R_Y_START_ADDR_L,
	R_X_END_ADDR_H, 		R_X_END_ADDR_L,
	R_Y_END_ADDR_H, 		R_Y_END_ADDR_L,
	R_SIZE_GAIN_ENABLE,
	R_X_OUTPUT_SIZE_H,		R_X_OUTPUT_SIZE_L,
	R_Y_OUTPUT_SIZE_H,		R_Y_OUTPUT_SIZE_L,
	R_X_OFFSET_H, 			R_X_OFFSET_L,
	R_Y_OFFSET_H, 			R_Y_OFFSET_L,
	//10.7
	R_BINNING_MODE,
	//10.8
	R_TEST_PATTERN_MODE,
	R_TEST_DATA_BLUE_H, 		R_TEST_DATA_BLUE_L,
	R_TEST_DATA_GB_H, 		R_TEST_DATA_GB_L,
	R_TEST_DATA_RED_H,		R_TEST_DATA_RED_L,
	R_TEST_DATA_GR_H, 		R_TEST_DATA_GR_L,
	R_TEST_PN9_SEED_H, 		R_TEST_PN9_SEED_L,
	//10.9  only selected Timing blok registers !!! TBD
	R_3DSYNC_CONFIG,
	R_CALI_THRESHOLD_H,		R_CALI_THRESHOLD_L,
	R_ALIGN_THRESHOLD_H,		R_ALIGN_THRESHOLD_L,
	R_EXPO_SYNC_CONFIG,
	R_TD_EXPOSYNC_H,		R_TD_EXPOSYNC_L,
	R_OFFSET_READSYNC_H,		R_OFFSET_READSYNC_L,
	R_READSYNC_DECODETHRESHOLD_H,	R_READSYNC_DECODETHRESHOLD_L,
	R_READSYNC_ERROR_FLAG,
	R_TS_CFG,
	R_TS_TARGET,
	R_TS_OFFSET_SEL,
	R_TS_VALID,
	R_TS_VALUE,
	R_OB_ENABLE,
	//10.10 BLC programming registers
	R_BLC_CFG ,
	//10.11 BLI registers
	//10.12 LSC programming registers
	//10.13 DPC programming registers
	//10.14 LSC parameter programming registers
	//10.15 OTP programming registers
	// 6.1 MIPI serial data interface
#if 0
	R_MIPI_EN,
	R_LANE_NUM,
	R_LANE_CFG,
#endif
	R_MIPI_EN,
	R_THS_PREPARE,
	R_TCLK_PREPARE,
	R_TLPX,
	R_THS_ZERO,
	R_THS_TRAIL,
	R_TCLK_ZERO,
	R_TCLK_TRAIL,
	R_TCLK_EXIT,
	R_TCLK_PRE_H,
	R_TCLK_PRE_L,
	R_TCLK_POST,
	R_FULL_TRIGGER_H,
	R_FULL_TRIGGER_L,
	R_LANE_CFG,
	R_LANE_FORMAT,
	R_LANE_NUM,

	R_REG_TOTAL_NUM
}himax_reg_e;
typedef enum opcode{
	RO = 0x01,				/* 0x03 mask for R W */
	RW = 0x02,
	WO = 0x04,
	EM = 0x10,		/* registers available in embedded data*/
	CMU = 0X20,		/* change to these registers requires an extra COMMAND UPDATE */
}himax_op_e;

//https://docs.google.com/spreadsheets/d/1XBnWvDMrwpQDqI3-03m6dyVSNDbeB1U-Y7f0VS1wdGc/edit?pli=1#gid=0
/*const */uint16_t addrtable[R_REG_TOTAL_NUM] = {  // each lines of addresses for each group of enum of  'himax_reg_e', each group of below 'regstable5530', 10+1 groups, for 11 lines
	0X0000, 0X0001, 0X0002, 0X0005, 0X0006,
	0x0100, 0x0101, 0x0103, 0x0104,
	0x0202, 0x0203, 0x0205, 0x020E, 0x020F,
	0X0303, 0X0304, 0X0305, 0X0307, 0X0309, 0X030A, 0X030D, 0X030F,
	0x0340, 0x0341, 0x0342, 0x0343,
	0x0344, 0x0345, 0x0346, 0x0347, 0x0348, 0x0349, 0x034A, 0x034B,   0x0350, 0x0351, 0x0352, 0x0353, 0x0354, 0x0355, 0x0356, 0x0357,    0x0358,
	0x0390,
	0x0601, 0x0602, 0x0603, 0x0604, 0x0605, 0x0606, 0x0607, 0x0608,    0x0609, 0x060A, 0x060B,
	0x2210, 0x221C, 0x221D, 0x221E, 0x221F, 0x2220, 0x2221, 0x2222,   0x2229, 0x222A, 0x222B, 0x222C, 0x222E, 0x2230, 0x2231, 0x2233,    0x2236, 0x2237, 0x2251,
	0x2407,
	0x2800, 0X2810, 0X2811, 0X2812, 0X2813, 0X2814, 0X2815, 0X2816,   0X281B, 0X281C, 0X281D, 0X281E, 0X2828, 0X2829, 0X2831, 0X2832,    0x2833
};

//#define STR_REG_ADDR_VALUE(x, w, v) {#x, addrtable[x], (uint8_t)(w),  (uint8_t)v}
//#define STR_REG_ADDR_VALUE(x, w, v) {#x, (himax_reg_e)x, (uint8_t)(w),  (uint8_t)v}
#define STR_REG_ADDR_VALUE(x, w, v) {#x, addrtable[(himax_reg_e)x], (uint8_t)(w),  (uint8_t)v}
#define STR_REG_ADDR_DATA(x, a, w, v) {#x, (uint16_t)(a), (uint8_t)(w),  (uint8_t)v}


typedef struct himax_reg_desc{
	char		name[60];		/* this will be removed, only use in debug*/
	uint16_t	addr;
	//himax_reg_e  addr;
	uint8_t 	op;
	uint8_t	value;
}himax_reg_desc_t;

//  "RO", the value should be the default setting by vendor; "RW" the value to write at default by Open Water; "WO" is the general ctrl per vendor requriement
/*
static himax_reg_desc_t regstable5530[R_REG_TOTAL_NUM] = {
	// 10.1 status registers
	STR_REG_ADDR_DATA(R_MODEL_ID_H, 	0X0000,	RO, 		0x55),
	STR_REG_ADDR_DATA(R_MODEL_ID_L, 	0X0001,	RO, 		0x30),
	STR_REG_ADDR_DATA(R_SILICON_REV, 	0X0002,	RO,		0x00),
	STR_REG_ADDR_DATA(R_FRAME_COUNT, 	0X0005,	RO|EM, 		0xFF),
	STR_REG_ADDR_DATA(R_PIXEL_ORDER, 	0X0006,	RO, 		0x02),
	//10.2 general setup registers
	STR_REG_ADDR_DATA(R_MODE_SELECT,  	0x0100,	RW, 		0x00), /* 0-standby, 1 streaming, 2 powerup*/
	STR_REG_ADDR_DATA(R_IMAGE_ORIENTATION,  0x0101,	RW|EM|CMU, 	0x00),  /* bit0 horizontal mirror, bit1 vertical flip, enable */
	STR_REG_ADDR_DATA(R_SW_RESET, 		0x0103,	WO, 		0xFF),
	STR_REG_ADDR_DATA(R_COMMAND_UPDATE,  	0x0104,	WO, 		0xFF),
	//10.3 output setup registers
	STR_REG_ADDR_DATA(R_COARSE_INTEGRATION_H,	0x0202,	RW|EM|CMU, 0x08),  /* time in lines 0x0876 = 2166 lines */
	STR_REG_ADDR_DATA(R_COARSE_INTEGRATION_L, 	0x0203,	RW|EM|CMU, 0x76),
	STR_REG_ADDR_DATA(R_ANALOG_GLOBAL_GAIN,		0x0205, RW|EM|CMU, 0x00),   /* 2^R[7:4] x (1+ R[3:0]/16),  e.g. OW x1, x4, x8, x16*/
	STR_REG_ADDR_DATA(R_DIGITAL_GLOBAL_GAIN_H, 	0x020E, RW|EM|CMU, 0x01),	/* 2.6 max = 0x0200, 2 integer bits*/
	STR_REG_ADDR_DATA(R_DIGITAL_GLOBAL_GAIN_L, 	0x020F,	RW|EM|CMU, 0x00),	/* 6 fraction bits,   0x20 = 32/64 for 6 bits = 0.5*/
	//10.4 clock setup regisers
	*/
#ifdef MCLK_19_2_MHZ || MCLK_19_2_MHZ_60HZ || MCLK_19_2_MHZ_80HZ
	//STR_REG_ADDR_DATA(R_VT_SYS_CLK_DIV, 	0X0303,		RW|CMU, 0x01),
	STR_REG_ADDR_DATA(R_VT_SYS_CLK_DIV, 	0X0303,		RW|CMU, 0x02),   /* 0 to 4 as 4/5/6/7/8, 1 for 5 in Gen2a 28Mhz setting; 0x02 Gen1 49.6*31*2/4/6=19.2*/
	STR_REG_ADDR_DATA(R_ADCPLL_CFG, 	0X0304,		RW|CMU, 0xA9),   /* both Gen1 Gen2 */
	//STR_REG_ADDR_DATA(R_PRE_PLL_CLK_DIV,  0X0305,		RW|CMU, 0x19),
	STR_REG_ADDR_DATA(R_PRE_PLL_CLK_DIV,  	0X0305,		RW|CMU, 0x04),   /*  Gen2 0x19 for 25Mhz, 0x1A for 26Mhz, 04 for 19.2Mhz*/
	//STR_REG_ADDR_DATA(R_PULTIPLIER, 	0X0307,		RW|CMU, 0x7C),
	STR_REG_ADDR_DATA(R_PULTIPLIER, 	0X0307,		RW|CMU, 0x1F),  /*  7C = 124   MCLK=26Mhz, vt_pix_clk=26*124/26/5*2 = 49.6Mhz   25Mhz*124/25/5*2=49.6Mhz 19.2*31*2/4/6=49.6*/
	STR_REG_ADDR_DATA(R_OP_PIX_CLK_DIV, 	0X0309,		RW|CMU, 0x00),  /* 00=01=1  b'10=b'11=2*/
	STR_REG_ADDR_DATA(R_MIPIPLL_CFG, 	0X030A,		RW|CMU, 0x0A),  /* */
	//STR_REG_ADDR_DATA(R_PRE_MIPI_PLL_CLK_DIV,0X030D,	RW|CMU, 0x19),
	STR_REG_ADDR_DATA(R_PRE_MIPI_PLL_CLK_DIV,0X030D,	RW|CMU, 0x0C),  /* 25Mhz using 25=0x19, 26Mhz using = 0x1A 19.2Mhz using 0x0C*/
	STR_REG_ADDR_DATA(R_PLL_MIPI_MULTIPLIER, 0X030F,	RW|CMU, 0x55),  /* 0x88 = 136  26MCLK*136/26*2 = 272 Mhz  25M*136/25*2=272MHz 0x55 is 85 19.2/12*85*2/1 = 272MHz*/
#elif MCLK_25MHZ
	STR_REG_ADDR_DATA(R_VT_SYS_CLK_DIV, 	0X0303,		RW|CMU, 0x02),
	STR_REG_ADDR_DATA(R_ADCPLL_CFG, 	0X0304,		RW|CMU, 0xA9),
	STR_REG_ADDR_DATA(R_PRE_PLL_CLK_DIV,  	0X0305,		RW|CMU, 0x0A),  /*  unique 0x0A for 25Mhz MCLK*/
	STR_REG_ADDR_DATA(R_PULTIPLIER, 	0X0307,		RW|CMU, 0x5B),  /*  unique 0x5B for 25Mhz MCLK*/
	STR_REG_ADDR_DATA(R_OP_PIX_CLK_DIV, 	0X0309,		RW|CMU, 0x00),
	STR_REG_ADDR_DATA(R_MIPIPLL_CFG, 	0X030A,		RW|CMU, 0x0A),
	STR_REG_ADDR_DATA(R_PRE_MIPI_PLL_CLK_DIV,0X030D,	RW|CMU, 0x0A),  /* unique 0x0A for 25Mhz MCLK */
	STR_REG_ADDR_DATA(R_PLL_MIPI_MULTIPLIER, 0X030F,	RW|CMU, 0x50),  /* unique 0x50 for 25Mhz MCLK */
#endif
	//10.5 frame timing registers
#ifdef MCLK_19_2_MHZ || MCLK_19_2_MHZ_60HZ || MCLK_19_2_MHZ_80HZ
	STR_REG_ADDR_DATA(R_FRAME_LENGTH_LINES_H, 0x0340,	RW|EM|CMU, 0x08),  /* Gen2 28Mhz 0x0B06=2822 = 2800 + 20 + 2, Gen1=0x0836=2102 lines */
	STR_REG_ADDR_DATA(R_FRAME_LENGTH_LINES_L, 0x0341,	RW|EM|CMU, 0x36),
	STR_REG_ADDR_DATA(R_LINE_LENGTH_PCK_H,    0x0342,	RW|EM|CMU, 0x0C),   /* Gen2 0x0C30 = 3120, Gen1=2880 = 0B84 */
	STR_REG_ADDR_DATA(R_LINE_LENGTH_PCK_L,    0x0343,	RW|EM|CMU, 0x30),
#elif MCLK_25MHZ
	STR_REG_ADDR_DATA(R_FRAME_LENGTH_LINES_H, 0x0340,	RW|EM|CMU, 0x08),  /* Gen2 28Mhz 0x0B06=2822 = 2800 + 20 + 2, Gen1=0x0836=2102 lines */
	STR_REG_ADDR_DATA(R_FRAME_LENGTH_LINES_L, 0x0341,	RW|EM|CMU, 0x36),
	STR_REG_ADDR_DATA(R_LINE_LENGTH_PCK_H,    0x0342,	RW|EM|CMU, 0x0C),   /* Gen2 0x0C30 = 3120, Gen1=2880 = 0B84 */
	STR_REG_ADDR_DATA(R_LINE_LENGTH_PCK_L,    0x0343,	RW|EM|CMU, 0x88),
#endif

	//10.6 image size registers
	STR_REG_ADDR_DATA(R_X_START_ADDR_H, 	0x0344, RW|CMU, 0),  /* (X=0,Y=0) */
	STR_REG_ADDR_DATA(R_X_START_ADDR_L, 	0x0345, RW|CMU, 0),
	STR_REG_ADDR_DATA(R_Y_START_ADDR_H, 	0x0346, RW|CMU, 0),
	STR_REG_ADDR_DATA(R_Y_START_ADDR_L, 	0x0347, RW|CMU, 0),
	STR_REG_ADDR_DATA(R_X_END_ADDR_H, 	0x0348, RW|CMU, 0x0A),  /* 0X0A9F  as 2719 i.e. 2720 effective pixels, 0x0A97=2711 Gen1 USB align*/
	//STR_REG_ADDR_DATA(R_X_END_ADDR_L, 	0x0349,	RW|CMU, 0x9F),  /* 2719 */ 
	STR_REG_ADDR_DATA(R_X_END_ADDR_L, 	0x0349,	RW|CMU, 0x97),  /* 2711*/
	STR_REG_ADDR_DATA(R_Y_END_ADDR_H, 	0x034A,	RW|CMU, 0x08),  /*  0x081F as 2079/2080 lines */
	STR_REG_ADDR_DATA(R_Y_END_ADDR_L, 	0x034B,	RW|CMU, 0x33),  /* 0x0833 as 2099/2100 */
	STR_REG_ADDR_DATA(R_SIZE_GAIN_ENABLE,   0x0350,	RW|CMU, 0xFF),
	STR_REG_ADDR_DATA(R_X_OUTPUT_SIZE_H,    0x0351,	RW|CMU, 0x0A),  /*  0x0AA0 = 2720 pixels  0x0A98=2712 USB packet align*/
	//STR_REG_ADDR_DATA(R_X_OUTPUT_SIZE_L, 	0x0352, RW|CMU, 0xA0),  /* 2720 */
	STR_REG_ADDR_DATA(R_X_OUTPUT_SIZE_L, 	0x0352, RW|CMU, 0x98),  /* 2712 */
	STR_REG_ADDR_DATA(R_Y_OUTPUT_SIZE_H,    0x0353,	RW|CMU, 0x08),  /* 0x0834 = 2100 lines in Gen2  0x0820=2080 lines in Gen1*/
	STR_REG_ADDR_DATA(R_Y_OUTPUT_SIZE_L, 	0x0354,	RW|CMU, 0x34),
    //STR_REG_ADDR_DATA(R_Y_OUTPUT_SIZE_L, 	0x0354,	RW|CMU, 0x20),
	STR_REG_ADDR_DATA(R_X_OFFSET_H, 	0x0355,	RW|CMU, 0), 		/* no offset */
	STR_REG_ADDR_DATA(R_X_OFFSET_L, 	0x0356,	RW|CMU, 0),
	STR_REG_ADDR_DATA(R_Y_OFFSET_H, 	0x0357,	RW|CMU, 0),
	STR_REG_ADDR_DATA(R_Y_OFFSET_L, 	0x0358,	RW|CMU, 0),
	//10.7binning mode
	STR_REG_ADDR_DATA(R_BINNING_MODE, 	0x0390,	RW|CMU, 0),
	//10.8 test pattern registers
	STR_REG_ADDR_DATA(R_TEST_PATTERN_MODE,	0x0601,	RW|CMU, 0),
	STR_REG_ADDR_DATA(R_TEST_DATA_BLUE_H, 	0x0602,	RW, 0),
	STR_REG_ADDR_DATA(R_TEST_DATA_BLUE_L, 	0x0603,	RW, 0),
	STR_REG_ADDR_DATA(R_TEST_DATA_GB_H, 	0x0604,	RW, 0),
	STR_REG_ADDR_DATA(R_TEST_DATA_GB_L, 	0x0605,	RW, 0),
	STR_REG_ADDR_DATA(R_TEST_DATA_RED_H, 	0x0606,	RW, 0),
	STR_REG_ADDR_DATA(R_TEST_DATA_RED_L, 	0x0607,	RW, 0),
	STR_REG_ADDR_DATA(R_TEST_DATA_GR_H, 	0x0608,	RW, 0),
	STR_REG_ADDR_DATA(R_TEST_DATA_GR_L, 	0x0609,	RW, 0),
	STR_REG_ADDR_DATA(R_TEST_PN9_SEED_H, 	0x060A,	RW, 0),
	STR_REG_ADDR_DATA(R_TEST_PN9_SEED_L, 	0x060B,	RW, 0),
	//10.9 timing block regsiters PARTIALLY
	STR_REG_ADDR_DATA(R_3DSYNC_CONFIG, 	0x2210,	RW, 0),
	STR_REG_ADDR_DATA(R_CALI_THRESHOLD_H, 	0x221C,	RW, 0x04),    /* 0x0400 as 1024*/
	STR_REG_ADDR_DATA(R_CALI_THRESHOLD_L, 	0x221D,	RW, 0x00),
	STR_REG_ADDR_DATA(R_ALIGN_THRESHOLD_H, 	0x221E,	RW, 0x01),   /* 0x01E0 = 460 half-rows*/
	STR_REG_ADDR_DATA(R_ALIGN_THRESHOLD_L, 	0x221F,	RW, 0xE0),
	STR_REG_ADDR_DATA(R_EXPO_SYNC_CONFIG, 	0x2220,	RW, 0x02),   /* origin 2 as read sync, temp 0 for no sync*/
	STR_REG_ADDR_DATA(R_TD_EXPOSYNC_H, 	0x2221,	RW, 0x00),
	STR_REG_ADDR_DATA(R_TD_EXPOSYNC_L, 	0x2222,	RW, 0x40),
	STR_REG_ADDR_DATA(R_OFFSET_READSYNC_H, 	0x2229,	RW, 0),
	STR_REG_ADDR_DATA(R_OFFSET_READSYNC_L, 	0x222A,	RW, 0),
	STR_REG_ADDR_DATA(R_READSYNC_DECODETHRESHOLD_H, 0x222B,	RW, 0xFF),   /* 0xFFFF=65535 half-rows,  Gen1 has 0x2080=8320 half-rows*/
	STR_REG_ADDR_DATA(R_READSYNC_DECODETHRESHOLD_L, 0x222C,	RW, 0xFF),
	STR_REG_ADDR_DATA(R_READSYNC_ERROR_FLAG, 	0x222E,	RO, 0),
	STR_REG_ADDR_DATA(R_TS_CFG, 			0x2230,	RW, 0),
	STR_REG_ADDR_DATA(R_TS_TARGET, 		0x2231,	RW, 0),
	STR_REG_ADDR_DATA(R_TS_OFFSET_SEL, 	0x2233,	RW, 0),
	STR_REG_ADDR_DATA(R_TS_VALID, 		0x2236,	RO, 0),
	STR_REG_ADDR_DATA(R_TS_VALUE, 		0x2237,	RO, 0),
	STR_REG_ADDR_DATA(R_OB_ENABLE, 		0x2251,	RW, 0x0F), 		/* Gen2 0x0F, Gen1 0x03   04.07 to turn off OB*/
	//10.10 BLC programming registers
	STR_REG_ADDR_DATA(R_BLC_CFG, 		0x2407,	RW, 0x46), 		/* disable BLC,  0x03 to enable*/
	//10.11 BLI registers
	//10.12 LSC programming registers
	//10.13 DPC programming registers
	//10.14 LSC parameter programming registers
	//10.15 OTP programming registers
	//  6.1 MIPI serial data interface
#if 0
	STR_REG_ADDR_DATA(R_MIPI_EN, 		0x2800,	RW, 0x01),	/* does these 2 need CMU ??  TBD*/
	STR_REG_ADDR_DATA(R_LANE_NUM, 		0x2833,	RW, 0x03),
	STR_REG_ADDR_DATA(R_LANE_CFG, 		0x2831,	RW, 0xDE)
#endif
	STR_REG_ADDR_DATA(R_MIPI_EN,		0X2800,  RW, 	0X01),
#ifdef MCLK_19_2_MHZ || MCLK_19_2_MHZ_60HZ || MCLK_19_2_MHZ_80HZ
	STR_REG_ADDR_DATA(R_THS_PREPARE,	0X2810,  RW, 	0X04),
	STR_REG_ADDR_DATA(R_TCLK_PREPARE,	0X2811,  RW, 	0X04),
	STR_REG_ADDR_DATA(R_TLPX,		0X2812,  RW, 	0X04),
	STR_REG_ADDR_DATA(R_THS_ZERO,		0X2813,  RW, 	0X09),
	STR_REG_ADDR_DATA(R_THS_TRAIL,		0X2814,  RW, 	0X06),
	STR_REG_ADDR_DATA(R_TCLK_ZERO,		0X2815,  RW, 	0X12),
	STR_REG_ADDR_DATA(R_TCLK_TRAIL,		0X2816,  RW, 	0X05),
	STR_REG_ADDR_DATA(R_TCLK_EXIT,		0X281B,  RW, 	0X05),
	STR_REG_ADDR_DATA(R_TCLK_PRE_H,		0X281C,  RW, 	0X05),
	STR_REG_ADDR_DATA(R_TCLK_PRE_L,		0X281D,  RW, 	0X00),
	STR_REG_ADDR_DATA(R_TCLK_POST,		0X281E,  RW, 	0X0B),
	STR_REG_ADDR_DATA(R_FULL_TRIGGER_H,	0X2828,  RW, 	0X00),
	STR_REG_ADDR_DATA(R_FULL_TRIGGER_L,	0X2829,  RW, 	0X33),
#elif MCLK_25_MHZ
	STR_REG_ADDR_DATA(R_THS_PREPARE,	0X2810,  RW, 	0X06),
	STR_REG_ADDR_DATA(R_TCLK_PREPARE,	0X2811,  RW, 	0X06),
	STR_REG_ADDR_DATA(R_TLPX,		0X2812,  RW, 	0X05),
	STR_REG_ADDR_DATA(R_THS_ZERO,		0X2813,  RW, 	0X0C),
	STR_REG_ADDR_DATA(R_THS_TRAIL,		0X2814,  RW, 	0X07),
	STR_REG_ADDR_DATA(R_TCLK_ZERO,		0X2815,  RW, 	0X1B),
	STR_REG_ADDR_DATA(R_TCLK_TRAIL,		0X2816,  RW, 	0X06),
	STR_REG_ADDR_DATA(R_TCLK_EXIT,		0X281B,  RW, 	0X08),
	STR_REG_ADDR_DATA(R_TCLK_PRE_H,		0X281C,  RW, 	0X08),
	STR_REG_ADDR_DATA(R_TCLK_PRE_L,		0X281D,  RW, 	0X00),
	STR_REG_ADDR_DATA(R_TCLK_POST,		0X281E,  RW, 	0X0C),
	STR_REG_ADDR_DATA(R_FULL_TRIGGER_H,	0X2828,  RW, 	0X00),
	STR_REG_ADDR_DATA(R_FULL_TRIGGER_L,	0X2829,  RW, 	0X10),
#endif
	STR_REG_ADDR_DATA(R_LANE_CFG, 		0x2831,	 RW, 	0xDE),
	STR_REG_ADDR_DATA(R_LANE_FORMAT, 	0x2832,	 RW, 	0x2B),
	STR_REG_ADDR_DATA(R_LANE_NUM, 		0x2833,	 RW, 	0x03)

};

/*  Example to access one register
		regstable5530[R_ANALOG_GLOBAL_GAIN].addr    set in the table, used directly,  no change
		regstable5530[R_ANALOG_GLOBAL_GAIN].value  can be changed
		regstable5530[R_ANALOG_GLOBAL_GAIN].op   can be checked
		regstable5530[R_ANALOG_GLOBAL_GAIN].name   can be printed out
	*/

#endif /* HIMAX5530_CONFIG_H_ */
