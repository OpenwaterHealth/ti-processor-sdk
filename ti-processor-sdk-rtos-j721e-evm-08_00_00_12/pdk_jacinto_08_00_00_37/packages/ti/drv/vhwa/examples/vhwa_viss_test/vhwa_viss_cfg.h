#ifndef APP_VISS_CFG_H_
#define APP_VISS_CFG_H_

Rfe_PwlConfig gPwlCfg[] =
{
    {
        4095,
        0,
        {-127, -127, -127, -127},
        {512, 512, 512, 512},
        TRUE,
        512, 1408, 2176,
        2048, 16384, 65536,
        4, 16, 64, 512,
        0, 1048575
    },
    {
        4095,
        0,
        {-240, -240, -240, -240},
        {512, 512, 512, 512},
        FALSE,
        512, 1024, 2048,
        512, 1024, 2048,
        2, 2, 2, 2,
        1, 1048575
    },
    {
        65535,
        0,
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        FALSE,
        512, 512, 2048,
        512, 1024, 2048,
        2, 2, 2, 2,
        1, 1048575
    },
};

Rfe_WdrConfig gWdrCfg[] =
{
    /* 0 */
    {
        FALSE, 0, 15, 4, 4,
        32768, 2048,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        4094,
        0, 0, 0,
        0, 65535,
        0, 262143
    },
    /* 1 */
    {
        FALSE, 0, 15, 8, 8,
        32768, 128,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        65504,
        0, 0, 0,
        0, 65535,
        0, 262143
    },
    /* 2 */
    {
        FALSE, 0, 15, 3, 3,
        32768, 5851,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        4956,
        0, 0, 0,
        0, 65535,
        0, 65535
    },
    /* 3 */
    {
        TRUE, 0, 15, 6, 3,
        32768, 5461,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        19786,
        0, 0, 0,
        0, 65535,
        0, 1048575
    },
    /* 4 */
    {
        TRUE, 0, 15, 3, 3,
        32768, 5851,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        4956,
        0, 0, 0,
        0, 65535,
        0, 65535
    },
    /* 5 */
    {
        TRUE, 0, 15, 6, 3,
        32768, 5461,
        {0, 0, 0, 0},
        {0, 0, 0, 0},
        {512, 512, 512, 512},
        {512, 512, 512, 512},
        19786,
        0, 0, 0,
        0, 65535,
        0, 1048575
    }
};

uint32_t gCompLut20To16[] = {
    #include "comp_lut_20_to_16_2.txt"
};
Vhwa_LutConfig gLutCfg[] =
{
    /* 0 */
    {
        FALSE, 16, 4095, NULL
    },
    /* 1 */
    {
        TRUE, 18, 65535, gCompLut20To16
    },
};

Rfe_DpcOtfConfig gDpcOtfCfg[] =
{
    {
        TRUE,
        {200, 200, 300, 500, 800, 1600, 3200, 6400},
        {0, 50, 50, 37, 50, 50, 50, 50}
    }
};

uint32_t gLscLut_2[] =
{
    #include "lsc_lut_2.txt"
};

Rfe_LscConfig gLscCfg[] =
{
    {
        FALSE,
        RFE_LSC_GAIN_FMT_U8Q8,
        RFE_LSC_DS_FACTOR_32,
        RFE_LSC_DS_FACTOR_16,
        NULL, 0
    },
    {
        TRUE,
        RFE_LSC_GAIN_FMT_U8Q8_1,
        RFE_LSC_DS_FACTOR_64,
        RFE_LSC_DS_FACTOR_64,
        gLscLut_2, 2232
    },
};

Rfe_GainOfstConfig  gWbCfg[] =
{
    /* WB CFG */
    {
        {512, 512, 512, 512},
        {0, 0, 0, 0}
    },
};

Fcp_CfaConfig       gCfaCfg[] =
{
    /* 0 */
    {
        {FALSE, FALSE, FALSE, FALSE},
        {
            #include "cfa_lut_colorb.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {0, 0, 0, 0},
            {0, 0, 0, 0}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        }
    },
    /* 1 */
    {
        {FALSE, FALSE, FALSE, FALSE},
        {
            #include "cfa_lut_3m.txt"
        },
        {FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0, FCP_CFA_CORE_SEL_CORE0,
            FCP_CFA_CORE_SEL_CORE0},
        {FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012,
            FCP_CFA_CORE_BLEND_MODE_INPUT012, FCP_CFA_CORE_BLEND_MODE_INPUT012},
        {
            {175, 95, 95, 175},
            {275, 195, 195, 275}
        },
        {
            {175, 95, 95, 175},
            {276, 196, 196, 276}
        },
        {
            {0, 1, 2, 3},
            {8, 9, 10, 11}
        },
        {
            {4, 5, 6, 7},
            {12, 13, 14, 15}
        },
        {
            {500, 600, 700, 800, 900, 1000, 1100},
            {0, 100, 200, 300, 400, 500, 600}
        }
    },
};

Fcp_CcmConfig   gCcmCfg[] =
{
    /* 0 */
    {
        {
            {367, -111, 0, 0},
            {-53,  372, -63, 0},
            {-8,  -164, 428, 0}
        },
        {
            0, 0, 0
        }
    },
    /* 1 */
    {
        {
            {453, -166, -31, 0},
            {-149,  420, -15, 0},
            {17,  -327, 566, 0}
        },
        {
            0, 0, 0
        }
    },
};

uint32_t gGammaTable[][FCP_GAMMA_LUT_SIZE] =
{
    {
        #include "saturation_lut.txt"
    },
    {
        #include "saturation_lut_3M.txt"
    }
};

Fcp_GammaConfig gGammaCfg[] =
{
    /* 0 */
    {
        TRUE,
        10,
        gGammaTable[0],
        gGammaTable[0],
        gGammaTable[0]
    },
    /* 1 */
    {
        TRUE,
        10,
        gGammaTable[1],
        gGammaTable[1],
        gGammaTable[1]
    },
};

Fcp_Rgb2HsvConfig   gRgb2HsvCfg[] =
{
    {
        FCP_RGB2HSV_INPUT_CONTRAST_OUTPUT,
        FCP_RGB2HSV_H1_INPUT_RED_COLOR,
        FCP_RGB2HSV_H2_INPUT_BLUE_COLOR,
        {64, 64, 128}, 0,
        0,
        {0, 0, 0},
        {0, 0, 0},
        0,
        FCP_SAT_MODE_SUM_RGB_MINUS_MIN_RGB,
        FCP_SAT_DIV_4096_MINUS_GREY
    },
};

Fcp_Rgb2YuvConfig   gRgb2YuvCfg[] =
{
    {
        {
            {77, 150, 29},
            {-44, -84, 128},
            {128, -108, -20}
        },
        {
            0, 128, 128
        }
    },
    /* RGB2YUV */
    {
        {
            {77, 150, 29},
            {-43, -85, 128},
            {128, -107, -21}
        },
        {
            0, 128, 128
        }
    },
};

Fcp_YuvSatLutConfig     gRgbLutCfg[] =
{
    {
        10,
        FALSE, NULL,
        FALSE, NULL,
        FALSE, NULL
    },
};

uint32_t gH3aLut[] =
{
    #include "h3a_16_to_10_lut.txt"
};

Glbce_PerceptConfig fwdPrcptCfg[] =
{
    {
        TRUE,
        {
            #include "glbce_fwd_percept_lut.txt"
        }
    }
};
Glbce_PerceptConfig revPrcptCfg[] =
{
    {
        TRUE,
        {
            #include "glbce_rev_percept_lut.txt"
        }
    }
};
Glbce_Config  glbceCfg[] =
{
    /* 0 */
    {
        255,        // Strenght_ir
        0,          // black level
        65535,      // white level
        0xC,        // intensity variance
        0x7,        // space variance
        0x6,
        0x6,
        GLBCE_NO_DITHER,
        72,
        62,
        {
            #include "glbce_asymmetry_lut.txt"
        }
    },
    /* 1 */
    {
        250,        // Strenght_ir
        0,          // black level
        65535,      // white level
        0xC,        // intensity variance
        0x7,        // space variance
        0xF,
        0x0,
        GLBCE_NO_DITHER,
        72,
        64,
        {
            #include "glbce_asymmetry_lut_150.txt"
        }
    },
    /* 2 */
    {
        250,        // Strenght_ir
        0,          // black level
        65535,      // white level
        0xC,        // intensity variance
        0x7,        // space variance
        0xF,
        0x0,
        GLBCE_NO_DITHER,
        72,
        64,
        {
            #include "glbce_asymmetry_lut_130.txt"
        }
    },
};

Nsf4v_Config nsf4Cfg[] =
{
    /* 0 */
    {
        {
            FALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10,
        {660, 512, 512, 762},
        32,
        {64, 32, 16},
        {
            {
                0   ,    16,    128,
                64  ,    20,    192,
                256 ,    38,    100,
                1024,    76,     52,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0
            },
            {
                0   ,    16,    128,
                64  ,    20,    192,
                256 ,    38,    100,
                1024,    76,     52,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0
            },
            {
                0   ,    16,    128,
                64  ,    20,    192,
                256 ,    38,    100,
                1024,    76,     52,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0
            },
            {
                0   ,    16,    128,
                64  ,    20,    192,
                256 ,    38,    100,
                1024,    76,     52,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0
            }
        }
    },
    /* 3 */
    {
        {
            FALSE,
            0,
            0, 0, 0, 0, 0, 0,
            {
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                },
                {
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0},
                    {0, 0, 0}
                }
            }
        },
        0x10u,
        {540, 512, 512, 1143},
        2,
        {64, 32, 16},
        {
            {
                0   ,    16,    128,
                64  ,    20,    192,
                256 ,    38,    100,
                1024,    76,     52,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0
            },
            {
                0   ,    16,    128,
                64  ,    20,    192,
                256 ,    38,    100,
                1024,    76,     52,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0
            },
            {
                0   ,    16,    128,
                64  ,    20,    192,
                256 ,    38,    100,
                1024,    76,     52,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0
            },
            {
                0   ,    16,    128,
                64  ,    20,    192,
                256 ,    38,    100,
                1024,    76,     52,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0,
                4096,     0,      0
            }
        }
    }
};

Rfe_H3aInConfig gRfeH3aInCfg[] =
{
    {
        RFE_H3A_IN_SEL_LSC_OUT_FRAME,       /* Input Source */
        0                                   /* Shift */
    }
};

Vhwa_LutConfig gRfeH3aLutCfg[] =
{
    {
        1, 16, 0x3FF,
        gH3aLut
    }                                   /* LUT config */
};

H3a_Config gH3aCfg[] =
{
    {
        H3A_MODULE_AEWB,
        // Position
        {
            0, 0,
        },
        // AEWB Config
        {
            FALSE, // ALAW
            FALSE, // MED Filter
            0x0,   // MEd Filter Threshold
            // Win Config
            {
                {
                    0, 30   // position
                },
                120, 32,  //size
                16, 32,  //Count
                8, 8,    // Incr
            },
            1078, 2,      // Black line start
            H3A_AEWB_OUTPUT_MODE_SUM_ONLY,   // mode
            0,          // sumshift
            1023        // sat limit
        },
        // AF Config
        {
        }
    },
    {
        H3A_MODULE_AF,
        // Position
        {
            0, 0,
        },
        // AEWB Config
        {
            FALSE, // ALAW
            FALSE, // MED Filter
            0x0,   // MEd Filter Threshold
            // Win Config
            {
                {
                    0, 30   // position
                },
                120, 32,  //size
                16, 32,  //Count
                8, 8,    // Incr
            },
            1078, 2,      // Black line start
            H3A_AEWB_OUTPUT_MODE_SUM_ONLY,   // mode
            0,          // sumshift
            1023       // sat limit
        },
        // AF Config
        {
            FALSE, /* enableALowCompr */
            TRUE, /*enableMedFilt */
            100,  /* midFiltThreshold*/
            H3A_AF_RGBPOS_GR_BG,  /* rgbPos */
            {
                /* vpsissH3aPaxelConfig_t*/
                {128u + 0, 16u + 1u}, /* Fvid2_PosConfig pos */
                (((1280u - 2u*128u)/3u) + 7u) & (~7u), /* width, rounded up to next multiple of 8*/
                (((720u - 2u*16u)/3u) + 7u) & (~7u), /* height, rounded up to next multiple of 8 */
                3u, /* horzCount */
                3u, /* vertCount*/
                8u, /* horzIncr */
                8u, /* vertIncr */
            },
            H3A_AF_FV_MODE_SUM, /* fvMode */
            H3A_AF_VF_VERT_HORZ, /* vfMode */
            { /* iirCfg1 */
                {8, -95, -49, 38, 76, 38, -111, -54, 17, -34, 17}, /* coeff */
                0 /* threshold */
            },
            { /* iirCfg2 */
                {11, -72, -50, 26, 51, 26, -92, -53, 19, -38, 19}, /* coeff */
                0
            },
            { /* firCfg1 */
                {3, -6, 0, 6, -3}, /* coeff */
                0 /* threshold */
            },
            { /* firCfg2 */
                {3, -6, 0, 6, -3}, /* coeff */
                0 /* threshold */
            },
            0 /* iirFiltStartPos */
        }
    }
};

int32_t gYeeLut[] = {
    #include "yee_lut.txt"
};

Fcp_EeConfig gEeCfg[] =
{
    {
        TRUE,
        FALSE,  // align for Y12
        FALSE,  // align for Y8
        0,      //Y1212
        FALSE,   // bypass
        TRUE,   // bypass
        TRUE,   // bypass
        TRUE,   // bypass

        0,      // left shift
        0,      // right shift

        4,      // yee shift
        {
            -1, -3, -5, -3, -2, 2, -5, 2, 48
        },
        0,      // y Thr
        0,      // merge sel
        1,      // Halo reduction
        0,
        0,
        0,
        0,
        0,
        gYeeLut
    },
};

Fcp_HistConfig gHistCfg[] = {
    {
        TRUE, FCP_HIST_IN_SEL_COLOR_RED,
        {0U, 0U, 1280U, 720U}
    }
};

AppViss_Cfg gVissCfg[] =
{
    /* 0, Used for Single Frame Input 720p StartColor B */
    {
        &gPwlCfg[0],        /* PWL VS */
        &gPwlCfg[0],        /* PWL S */
        &gPwlCfg[0],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[0],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[0],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */
        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[0],        /* CFA */

        &gCcmCfg[0],        /* CCM */

        &gGammaCfg[0],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[0],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[0],        /* NSF4 */
        &glbceCfg[0],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[1],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 1, Two Frame Merge */
    {
        &gPwlCfg[1],        /* PWL VS */
        &gPwlCfg[2],        /* PWL S */
        &gPwlCfg[1],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[2],        /* WDR1 */
        &gWdrCfg[3],        /* WDR1 */
        &gLutCfg[1],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[1],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[1],        /* CFA */

        &gCcmCfg[1],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[1],        /* NSF4 */
        &glbceCfg[1],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        FALSE,              /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 2, Three Frame Merge with AEWB */
    {
        &gPwlCfg[1],        /* PWL VS */
        &gPwlCfg[1],        /* PWL S */
        &gPwlCfg[1],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[4],        /* WDR1 */
        &gWdrCfg[5],        /* WDR1 */
        &gLutCfg[1],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[1],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[1],        /* CFA */

        &gCcmCfg[1],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[1],        /* NSF4 */
        &glbceCfg[2],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[0],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 3, Three Frame Merge with AF */
    {
        &gPwlCfg[1],        /* PWL VS */
        &gPwlCfg[1],        /* PWL S */
        &gPwlCfg[1],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[4],        /* WDR1 */
        &gWdrCfg[5],        /* WDR1 */
        &gLutCfg[1],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[1],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[1],        /* CFA */

        &gCcmCfg[1],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[1],        /* NSF4 */
        &glbceCfg[2],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[1],        /* H3A */
        NULL,               /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 4, Two Frame Merge */
    {
        &gPwlCfg[1],        /* PWL VS */
        &gPwlCfg[2],        /* PWL S */
        &gPwlCfg[1],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[2],        /* WDR1 */
        &gWdrCfg[3],        /* WDR1 */
        &gLutCfg[1],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[1],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[1],        /* CFA */

        &gCcmCfg[1],        /* CCM */

        &gGammaCfg[1],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[1],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[1],        /* NSF4 */
        &glbceCfg[1],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        FALSE,              /* H3A */
        &gEeCfg[0],         /* EE */
        NULL,               /* Hist Cfg*/
    },
    /* 5, Used for Single Frame Input 720p StartColor B, histogram enabled */
    {
        &gPwlCfg[0],        /* PWL VS */
        &gPwlCfg[0],        /* PWL S */
        &gPwlCfg[0],        /* PWL L */
        NULL,               /* PWL LUT VS */
        NULL,               /* PWL LUT S */
        NULL,               /* PWL LUT L */
        &gWdrCfg[0],        /* WDR1 */
        &gWdrCfg[1],        /* WDR1 */
        &gLutCfg[0],        /* 20 to 16 LUT config */
        &gDpcOtfCfg[0],     /* DPC OTF */
        NULL,               /* DPC OTF */
        &gLscCfg[0],        /* LSC */
        &gWbCfg[0],         /* WB */
        &gRfeH3aInCfg[0],   /* RFE H3A Config */
        &gRfeH3aLutCfg[0],  /* H3a LUT */

        &gLutCfg[0],        /* 16 to 12 LUT */

        &gCfaCfg[0],        /* CFA */

        &gCcmCfg[0],        /* CCM */

        &gGammaCfg[0],      /* Gamma */

        &gRgb2HsvCfg[0],    /* RGB2HSV */

        &gRgb2YuvCfg[0],    /* RGB2YUV */

        &gRgbLutCfg[0],     /* RGB Lut */

        &nsf4Cfg[0],        /* NSF4 */
        &glbceCfg[0],       /* GLBCE */
        &fwdPrcptCfg[0],    /* Glbce_fwdPercept */
        &revPrcptCfg[0],    /* Glbce_revPercept */
        &gH3aCfg[1],        /* H3A */
        NULL,               /* EE */
        &gHistCfg[0],       /* Hist Cfg*/
    },
};

AppViss_TestConfig gAppVissTestConfig[] =
{
    /* 0, Used for Single Frame Input 720p StartColor B */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280*2, 1280*2, 1280*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    TRUE,
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {FALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[0]
        }
    },
    /* 1, Two Frame Merge wth two YUV420 and saturation output */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_TWO_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 0}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    TRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_SATURATION, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                /* H3A Output */
                {
                    FALSE,
                    {
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[1]
        }
    },
    /* 2, Three Frame Merge wth two YUV420, saturation and AEWB output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    TRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_SATURATION, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                /* H3A Output */
                {
                    TRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[2]
        }
    },
    /* 3, Three Frame Merge wth two YUV420, saturation and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    TRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_SATURATION, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                /* H3A Output */
                {
                    TRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[3]
        }
    },
    /* 4, Three Frame Merge wth YUV420, RGB and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_RGB24_888_PLANAR, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    TRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[3]
        }
    },
    /* 5, Three Frame Merge wth YUV420, HSV and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                        FVID2_DF_GREY, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_SATURATION, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                /* H3A Output */
                {
                    TRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[3]
        }
    },
    /* 6, Two Frame Merge with two YUV420 and saturation output, EE enabled on Y12 */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_TWO_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 0}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    FALSE,
                    {
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_ON_LUMA12,     /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[4]
        }
    },
    /* 7, Two Frame Merge with two YUV420 and saturation output, EE enabled on Y8 */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_TWO_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 0}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    FALSE,
                    {
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_ON_LUMA8,      /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[4]
        }
    },
    /* 8, Three Frame Merge with YUV420, YUV422(UYVY) and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    FALSE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920, 1920}, {FALSE},
                        FVID2_DF_YUV422I_UYVY, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    TRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[3]
        }
    },
    /* 9, Three Frame Merge with YUV420, YUV422(YUYV) and AF output  */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_THREE_FRAME_MERGE,
            /* In Format */
            {
                0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS16_PACKED
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    FALSE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920*2, 1920*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1920, 1080, {1920*2, 1920, 1920}, {FALSE},
                        FVID2_DF_YUV422I_YUYV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    TRUE,
                    {
                        0, 1920, 1080, {1920, 1920, 1920}, {FALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[3]
        }
    },
    /* 10, Used for Single Frame Input 720p StartColor, with histogram output */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280*2, 1280*2, 1280*2}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS12_UNPACKED16
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    TRUE,
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {FALSE},
                        FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[5]
        }
    },
    /* 11, Used for Single Frame Input 720p StartColor B */
    {
        /* VISS Parameters */
        {
            /* Input Mode */
            VHWA_M2M_VISS_MODE_SINGLE_FRAME_INPUT,
            /* In Format */
            {
                0, 1280, 720, {1280*2, 0, 0}, {FALSE},
                FVID2_DF_RAW, FVID2_SF_PROGRESSIVE,
                FVID2_CCSF_BITS12_UNPACKED16
            },
            /* Output Parameters */
            {
                {
                    /* Output Enabled */
                    FALSE,
                    /* Output Format */
                    {
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    /* Output Enabled */
                    TRUE,
                    /* Output Format */
                    {
                        0, 1280, 720, {1280, 1280, 1280}, {FALSE},
                        FVID2_DF_YUV420SP_UV, FVID2_SF_PROGRESSIVE,
                        FVID2_CCSF_BITS8_PACKED
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                {
                    FALSE,
                    {
                    }
                },
                /* H3A Output */
                {
                    FALSE
                },
            },
            TRUE,                           /* enable GLBCE */
            TRUE,                           /* Enable NSF4 */
            VHWA_M2M_VISS_EE_DISABLE,       /* EE Mode */
            TRUE                            /* Enable DPC */
        },
        /* VISS config */
        {
            &gVissCfg[0]
        }
    },
};

AppViss_TestParams gAppVissTestPrms[] =
{
    {
        "TC_001",                   /* Test Name */
        1,                          /* Num Handles */
        100,                        /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[0]},     /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_002",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[1]},     /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_003",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[2]},     /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_004",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[3]},     /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_005",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[4]},     /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_006",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[5]},     /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_007",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[6]},     /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_008",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[7]},     /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_009",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[8]},     /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_010",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[9]},   /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
    {
        "TC_011",                   /* Test Name */
        1,                          /* Num Handles */
        3,                          /* Repeate Count */
        TRUE,                       /* Is Performance */
        {&gAppVissTestConfig[10]},   /* Test Config */
        TRUE,
        FALSE,
        FALSE
    },
};



#endif

