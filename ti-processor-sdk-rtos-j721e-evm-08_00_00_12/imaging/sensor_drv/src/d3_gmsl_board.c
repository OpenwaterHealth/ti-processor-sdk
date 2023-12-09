/*
 * Support for the TDA4 GMSL board
 * Copyright (c) 2020, D3 Engineering.  All rights reserved.
 */

#include <stdint.h>
#include <stdio.h>
#include <pcal9539.h>
#include <d3_gmsl_board.h>
#include "max9296.h"
#include <iss_sensors.h>
#include <iss_sensor_if.h>
#include <iss_sensor_priv.h>
#include <app_remote_service.h>
#include <app_ipc.h>



/* Configure the pin mux for i2c6. */
void d3_i2c6_pinmux(void)
{
    ;
}

/* Control the power over coax.
 * For now, turn everything on.
 * Later, let things be turned off.
 * The hardware allows control over each coax. individually.
 */
int32_t d3_gmsl_poc_power(void *i2c_handle, uint8_t channel_mask)
{
    int32_t status = 0;
    const uint8_t global_powerPort = 1;
    const uint8_t global_powerPin = 4;
    uint8_t port;
    uint8_t bits;
    uint8_t bit_mask;

    /* VDD_DSS is the global 12V.
     * It is controlled by the gpio expander at 0x75 pin 1.4
     */
    port = global_powerPort;
    bit_mask = 1 << global_powerPin;

    bits = PCAL9539_DIRECTION_OUTPUT << global_powerPin;
    status |= pcal9539_setDirection(i2c_handle, 0x75, port, bits, bit_mask);

    bits = 1 << global_powerPin;
    status |= pcal9539_setOutValue(i2c_handle, 0x75, port, bits, bit_mask);

    /* The power on each coax. cable is controlled by the gpio expander
     * at 0x74, pins 0.0 to 0.3
     * | Pin | Conn. | Sensor   | Ser. | GMSL Port |
     * |-----|-------|----------|------|-----------|
     * |   0 | J10   | not used | 0x68 |     B     |
     * |   1 | J4    | not used | 0x6C |     B     |
     * |   2 | J7    | 0        | 0x68 |     A     |
     * |   3 | J2    | 1        | 0x6C |     A     |
     */
    port = 0;
    bits = 0;
    bit_mask = 0x0F;
    bits = (PCAL9539_DIRECTION_OUTPUT << 3) | (PCAL9539_DIRECTION_OUTPUT << 2)
         | (PCAL9539_DIRECTION_OUTPUT << 1) | (PCAL9539_DIRECTION_OUTPUT << 0);
    status |= pcal9539_setDirection(i2c_handle, 0x74, port, bits, bit_mask);

    channel_mask &= 0x03;
    if (channel_mask & 1) {
        bit_mask |= (1 << 2) | (1 << 0);
    }
    if (channel_mask & 2) {
        bit_mask |= (1 << 3) | (1 << 1);
    }

    bits = bit_mask;
    status |= pcal9539_setOutValue(i2c_handle, 0x74, port, bits, bit_mask);
    return status;
}

/* Control the MAX9296 power down pin. */
int32_t d3_gmsl_des_pwdn(void *i2c_handle, uint32_t channel_mask)
{
    int32_t status = 0;
    const uint8_t gpio_i2c_addr = 0x76;
    uint8_t port = 0;
    uint8_t bits;
    uint8_t mask;
    uint8_t pin = 4;

    /* The GMSL_PWDN pin of the two MAX9296 are controlled by the gpio
     * expander at 0x76, port0, pin4 and port1, pin4
     */
    mask = (1 << pin);
    bits = (PCAL9539_DIRECTION_OUTPUT << pin);
    port = 0;
    status |= pcal9539_setDirection(i2c_handle, gpio_i2c_addr, port, bits, mask);
    port = 1;
    status |= pcal9539_setDirection(i2c_handle, gpio_i2c_addr, port, bits, mask);

    bits = mask;
    if (channel_mask & 1) {
        port = 0;
        status |= pcal9539_setOutValue(i2c_handle, gpio_i2c_addr, port, bits, mask);
    }
    if (channel_mask & 2) {
        port = 1;
        status |= pcal9539_setOutValue(i2c_handle, gpio_i2c_addr, port, bits, mask);
    }
    return status;
}

uint8_t d3_gmsl_board_get_i2c_bus_id(uint8_t chId)
{
    return D3_GMSL_BOARD_I2C_BUS_ID;
}

/* MAX9296 @0x68:
 *   GMSL2 is portA
 *   GMAL0 is portB
 *   portA => CSI0, 4 lanes
 * MAX9296 @0x6C:
 *   GMSL3 is portA
 *   GMSL1 is portB
 *   portA => CSI1, 4 lanes
 * At this time we only support cameras on portA.
 */
static uint8_t max9296_i2c_addresses[] = { 0x68, 0x6C };

int8_t  d3_gmsl_board_get_des_addr(uint8_t chId)
{
    uint8_t n_addr = sizeof(max9296_i2c_addresses) / sizeof(max9296_i2c_addresses[0]);
    if (chId < n_addr)
        return max9296_i2c_addresses[chId];
    return -1;
}

/* Disable the GMSL link on all deserializers except the selected one.
 * We're doing that by selecting the link B, which is not used
 * (or, at least we only expect the channel A to be connected to a serializer)
 * There are only 2 deserializers on this board, so "all the other deserializers"
 * means !ch_id.
 */
int32_t d3_gmsl_board_disable_all_other_channels(void *i2c_handle, uint8_t ch_id)
{
    return max9296_select_gmsl_link(i2c_handle, !ch_id, MAX9296_LINK_MODE_B);
}

int32_t d3_gmsl_board_enable_all_other_channels(void *i2c_handle, uint8_t ch_id)
{
    return max9296_select_gmsl_link(i2c_handle, !ch_id, MAX9296_LINK_MODE_A);
}
