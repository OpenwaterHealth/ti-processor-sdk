/*
 *
 *
 */
#include <iss_sensors.h>
#include <iss_sensor_priv.h>
#include <iss_sensor_if.h>
#include <iss_sensor_serdes.h>
/**< ISS AEWB plugin is included here to get the default AEWB configuration
     from each sensor */
     
// #define HIMAX5530_OUT_WIDTH   (2720) // Default is 2720 (Gen1b used 2712 due to USB bit packing)

#define HIMAX5530_OUT_WIDTH   (1920) // Default is 2720 (Gen1b used 2712 due to USB bit packing)
#define HIMAX5530_OUT_HEIGHT  (1450) // Default is 2100 (2080 rows + 20 OB rows)
// #define HIMAX5530_OUT_HEIGHT  (920)  // 19MHz60hz 920x2720
// #define HIMAX5530_OUT_HEIGHT  (654)  // 19MHz80hz 654x2720

// to change width and height output we change teh X_OUTPUT_SIZE registers 0x0351, 0x0352
// and the X_OFFSET registers 0x0355, 0x0356 (same would be true to modify the Y output our tests just removed the outer columns)

/*******************************************************************************
 *  Defines
 *******************************************************************************
 */
#define ISS_SENSOR_HIMAX5530_FEATURES   (ISS_SENSOR_FEATURE_MANUAL_EXPOSURE|   \
                                         ISS_SENSOR_FEATURE_MANUAL_GAIN| \
                                         ISS_SENSOR_FEATURE_LINEAR_MODE)


// TODO: Find this out
#define HIMAX5530_CHIP_ID_REG_ADDR (0x0000)
#define HIMAX5530_CHIP_ID_REG_VAL  (0x55)

// For the moment these are commented about because they're copied from the IMX390
/*
#define ISS_HIMAX5530_GAIN_TBL_STEP_SIZE           (100U)
#define ISS_HIMAX5530_GAIN_TBL_STARTOFFSET         (10U)
#define ISS_HIMAX5530_MAX_INTG_LINES               (2050U)
#define ISS_HIMAX5530_VMAX                         (0x44c)
#define ISS_HIMAX5530_RHS                          (0x85U)

#define HIMAX5530_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR         (0x0018U)
#define HIMAX5530_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR_HIGH    ( \
        HIMAX5530_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR + 1U)
#define HIMAX5530_SP1L_ANALOG_GAIN_CONTROL_REG_ADDR         (0x001AU)
#define HIMAX5530_SP1L_ANALOG_GAIN_CONTROL_REG_ADDR_HIGH    ( \
        HIMAX5530_SP1L_ANALOG_GAIN_CONTROL_REG_ADDR + 1U)

#define HIMAX5530_VMAX                         (0x465U)

#define HIMAX5530_AE_CONTROL_LONG_REG_ADDR_LOW   (0x000C)
#define HIMAX5530_AE_CONTROL_LONG_REG_ADDR_HIGH   ( \
        HIMAX5530_AE_CONTROL_LONG_REG_ADDR_LOW + 1)
#define HIMAX5530_AE_CONTROL_LONG_REG_ADDR_TOP   ( \
        HIMAX5530_AE_CONTROL_LONG_REG_ADDR_HIGH + 1)

#define HIMAX5530_AE_CONTROL_SHORT_REG_ADDR_LOW   (0x0010)
#define HIMAX5530_AE_CONTROL_SHORT_REG_ADDR_HIGH  ( \
        HIMAX5530_AE_CONTROL_SHORT_REG_ADDR_LOW + 1)
#define HIMAX5530_AE_CONTROL_SHORT_REG_ADDR_MSB   ( \
        HIMAX5530_AE_CONTROL_SHORT_REG_ADDR_HIGH + 1)


#define HIMAX5530_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR         (0x0018U)
#define HIMAX5530_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR_HIGH    ( \
        HIMAX5530_SP1H_ANALOG_GAIN_CONTROL_REG_ADDR + 1U)

#define HIMAX5530_SP1L_ANALOG_GAIN_CONTROL_REG_ADDR         (0x001AU)
#define HIMAX5530_SP1L_ANALOG_GAIN_CONTROL_REG_ADDR_HIGH    ( \
        HIMAX5530_SP1L_ANALOG_GAIN_CONTROL_REG_ADDR + 1U)
*/

/*******************************************************************************
 *  Data structure's
 *******************************************************************************
 */

struct {

    uint32_t                    maxCoarseIntgTime;
    /**< Max Coarse integration time in milliseconds supported by sensor */
    uint32_t                     lineIntgTime;
    /**< Line Integration time in microseconds */
    uint32_t                     pixIntgTime;
    /**< Pixel Integration time in microseconds  */
} gHimax5530DeviceObj;

/*******************************************************************************
 *  Local Functions Declarations
 *******************************************************************************
 */

static int32_t HIMAX5530_Probe(uint32_t chId, void *pSensorHdl);
static int32_t HIMAX5530_Set_Gain(uint32_t i2cInstId, uint8_t sensorI2cAddr, double gain_dec);
static int32_t HIMAX5530_Config(uint32_t chId, void *pSensorHdl, uint32_t sensor_features_requested);
static int32_t HIMAX5530_StreamOn(uint32_t chId, void *pSensorHdl);
static int32_t HIMAX5530_StreamOff(uint32_t chId, void *pSensorHdl);
static int32_t HIMAX5530_PowerOn(uint32_t chId, void *pSensorHdl);
static int32_t HIMAX5530_PowerOff(uint32_t chId, void *pSensorHdl);
static int32_t HIMAX5530_GetExpParams(uint32_t chId, void *pSensorHdl, IssSensor_ExposureParams *pExpPrms);
static int32_t HIMAX5530_SetAeParams(void *pSensorHdl, uint32_t chId, IssSensor_ExposureParams *pExpPrms);
static int32_t HIMAX5530_GetDccParams(uint32_t chId, void *pSensorHdl, IssSensor_DccParams *pDccPrms);
static void HIMAX5530_InitAewbConfig(uint32_t chId, void *pSensorHdl);
static void HIMAX5530_GetIspConfig (uint32_t chId, void *pSensorHdl);
static void HIMAX5530_deinit (uint32_t chId, void *pSensorHdl);
static int32_t HIMAX5530_ReadWriteReg (uint32_t chId, void *pSensorHdl, uint32_t readWriteFlag, I2cParams *pReg);
static int32_t HIMAX5530_GetExpPrgFxn(uint32_t chId, void *pSensorHdl, IssAeDynamicParams *p_ae_dynPrms);

static int32_t HIMAX5530_WriteReg(uint8_t    i2cInstId,
                             uint8_t       i2cAddr,
                             uint16_t         regAddr,
                             uint8_t          regValue,
                             uint32_t      numRegs);

static int32_t HIMAX5530_ReadReg(uint8_t      i2cInstId,
                            uint8_t         i2cAddr,
                            uint16_t        regAddr,
                            uint8_t         *regVal,
                            uint32_t        numRegs);
