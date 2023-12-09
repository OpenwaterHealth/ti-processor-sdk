#include "ar0820_serdes_config.h"

I2cParams ub953SerCfg_AR0820[AR0820_SER_CFG_SIZE] = {
    {0x1, 0x2, 1000},
    {0x0E, 0xF0, 0x1},
    {0x0D, 0x0F, 0x1},
    {0xFFFF, 0x00, 0x0} /*End of script */
};

I2cParams ub960DesCfg_AR0820[AR0820_DES_CFG_SIZE] = {
    {0x1, 0x2, 1000},
    {0xB3, 0x0, 0x1},
    {0x4C, 0x01, 0x1},
    {0x58, 0x5e, 0x1},
    {0x5C, (SER_0_I2C_ALIAS << 1), 0x10},
    {0x5D, (SENSOR_0_I2C_ALIAS << 1), 0x10},
    {0x65, (SER_0_I2C_ALIAS << 1), 0x10},
    {0x5E, (AR0820_I2C_ADDR << 1), 0x10},
    {0x66, (SENSOR_0_I2C_ALIAS << 1), 0x10},
    {0x32, 0x01, 0x1},
    {0x33, 0x02, 0x1},
    {0x20, 0x20, 0x1},
    {0x0E, 0x7F, 0x1},
    {0x6E, 0x10, 0x1},
    {0x6F, 0x32, 0x1},
    {0xFFFF, 0x00, 0x0} /*End of script */
};

I2cParams ub960AR0820DesCSI2Enable[2u] = {
    {0x33, 0x03, 0x10},
    {0xFFFF, 0x00, 0x0} //End of script
};

I2cParams ub960AR0820DesCSI2Disable[2u] = {
    {0x33, 0x02, 0x10},
    {0xFFFF, 0x00, 0x0} /*End of script */
};

/* TODO: The serializer and deserializer settings are not sensor-specific.
 * It makes sense to move them in their own files.
 */

I2cParams max9296DesCfg[] = {
    { 0x0001, 0x02, 0 }, /* link speed = 6Gbps */
    { 0x0010, 0x31, 100 }, /* RESET_ONE_SHOT | AUTO_LINK */
    { 0x0330, 0x04, 0 }, /* MIPI_PHY0 set des in 2 x 4 mode */
    { 0x0333, 0x4E, 0 }, /* MIPI_PHY3 Lane Mapping register 1 */
    { 0x0334, 0xE4, 0 }, /* MIPI_PHY4 Lane Mapping register 2 */
    { 0x040A, 0x00, 0 }, /* MIPI_TX10 PHY0 Lane Count Register not used in 2 x 4 */
    { 0x044A, 0x40, 0 }, /* MIPI_TX10 PHY1 Lane Count Register in Port A */
    { 0x048A, 0x40, 0 }, /* MIPI_TX10 PHY2 Lane Count Register in Port B */
    { 0x04CA, 0x00, 0 }, /* MIPI_TX10 PHY3 Lane Count register unused in 2 x 4 */
    { 0x031D, 0x30, 0 }, /* BACKTOP22 PHY0 MIPI Clock Rate not used in 2 x 4 */
    { 0x0320, 0x30, 0 }, /* BACKTOP25 PHY1 MIPI Clock Rate Port A = 600 MHz */
    { 0x0323, 0x30, 0 }, /* BACKTOP28 PHY2 MIPI Clock Rate Port B */
    { 0x0326, 0x30, 0 }, /* BACKTOP31 PHY3 MIPI Clock Rate not used in 2 x 4 */
    { 0x0050, 0x00, 0 }, /* RX0 Pipe X Stream Select */
    { 0x0051, 0x01, 100 }, /* RX0 Pipe Y Stream Select */
    { 0x0313, 0x02, 0 }, /* BACKTOP12 Enable MAX9296 MIPI CSI */
    { 0xFFFF, 0x00, 0x0 } /*End of script */
};

I2cParams max9295SerCfg[] = {
    { 0x0001, 0x08, 0 }, /* link speed 6Gbps */
    { 0x0010, 0x21, 100 }, /* Reset one shot */
    { 0x0330, 0x00, 0 }, /* MIPI_RX0 Set PHY to 1x4 mode*/
    { 0x0331, 0x33, 0 }, /* MIPI_RX1 Enable Port A & B as they are shared in MAX9295A*/
    { 0x0308, 0x6F, 0 }, /* FRONTTOP_0 CSI Port selection Port B active in 1 x 4 mode*/
    { 0x0311, 0x20, 0 }, /* FRONTTOP_9 Start video pipe Y*/
    { 0x0314, 0x22, 0 }, /* FRONTTOP_12 Pipe X unused*/
    { 0x0316, 0x6C, 0 }, /* FRONTTOP_14 Datatype RAW12*/
    { 0x0318, 0x22, 0 }, /* FRONTTOP_16 Pipe Z unused*/
    { 0x031A, 0x22, 0 }, /* FRONTTOP_18 Pipe U unused*/
    { 0x02BE, 0x18, 0 }, /* GPIO_OUT = 1, GPIO_RX_EN = 0, GPIO_OUT_DIS = 0 */
    { 0x02BF, 0X20, 0 }, /* OUT_TYPE = 1 (push-pull) */
    { 0x0002, 0xF3, 0 }, /* REG2 Make sure all pipes start transmission*/
    { 0xFFFF, 0x00, 0x0 } /*End of script */
};

I2cParams max9296DesCSI2Enable[] = {
    { 0x313, 0x62, 0 },
    { 0xFFFF, 0x00, 0 }
};

I2cParams max9296DesCSI2Disable[] = {
    { 0x313, 0x00, 0 },
    { 0xFFFF, 0x00, 0 }
};
/* TODO:
 * Sequence that does the i2c address remapping.
 */

