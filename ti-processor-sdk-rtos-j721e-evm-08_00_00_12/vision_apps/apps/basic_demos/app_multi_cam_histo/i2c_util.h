/*
    I2C Command utility.  
    This utility takes a bridge setting, i2c address, i2c command, and payload 
    and sends them down the i2c line(s) on the Openwater device.

	Date:  6 June 2022  erik@openwater.cc

    Originally based upon: lsw_laser_touch_pdu_main.c

    How to build this separated system app on TDA4 Linux user space:
             - scp this file to a TDA4 directory, e.g. /opt/vision_apps
             > scp i2c_cmd.c root@192.168.0.183:/opt/vision_apps
             - cd to that directory,  
             > gcc -o ./i2ccmd i2c_util.c -l pthread
             - then the program can be run with ./i2ccmd	
*/

/******************************************************************************
 * Constant Definitions
 *****************************************************************************/
// Version 1.x was the original lsw_laser_touch_pdu_main.c
#define VERSION_MAJOR               2       
#define VERSION_MINOR               0

// I2C
#define I2C_ADDR_BRIDGE             0x71    // was 0X74
#define I2C_BUS_PATH                "/dev/i2c-6"
#define I2C_MAX_PAYLOAD_SIZE_BYTES  16
#define I2C_READ_LEN_BYTES          0x04

/******************************************************************************
 * Enums
 *****************************************************************************/
// For use in all fcns(), add failure modes as needed
typedef enum function_return_values {
    RET_SUCCESS = 0,
    RET_FAILED_TO_OPEN_I2C_BUS,
    RET_I2C_BRIDGE_SLAVE_ADDR_FAILED,
    RET_LASER_SLAVE_ADDR_FAILED,
    RET_GENERAL_FAIL
} fcn_ret_val;

/******************************************************************************
 * Data Structs
 *****************************************************************************/
struct i2c_cmd_frame {
    uint8_t bridge_address;
    uint8_t bridge_setting;
    uint8_t payload[I2C_MAX_PAYLOAD_SIZE_BYTES];  // Byte 0 = command
    uint8_t payload_length;
    int     response_val;
};
