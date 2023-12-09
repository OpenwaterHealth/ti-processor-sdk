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

#include <stdint.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <asm/ioctls.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include <unistd.h>         // write() cmd
#include <ctype.h>          // warning: implicit declaration of function tolower
#include <stdbool.h>        // error: unknown type name bool
#include <fcntl.h>          // error: O_RDWR undeclared 
#include "i2c_util.h"

/******************************************************************************
 * invoke_cmd()
 * 
 * Recv: i2c_cmd_fram structure containig the data for the i2c command.
 * Rets:  * Rets: fcn_ret_val indicating success/failure of execution
 *****************************************************************************/ 
fcn_ret_val invoke_cmd(struct i2c_cmd_frame i2c_cmd) {
    struct i2c_cmd_frame first_read_buf;
    struct i2c_cmd_frame write_buf;
    struct i2c_cmd_frame second_read_buf; 

    printf("Debug: addr: 0x%x set: 0x%x len:%d, data: addr: 0x%x cmd: 0x%x payload: 0x%x 0x%x 0x%x 0x%x 0x%x \n",  \
            i2c_cmd.bridge_address, i2c_cmd.bridge_setting, i2c_cmd.payload_length, 
            i2c_cmd.payload[0], 
            i2c_cmd.payload[1], i2c_cmd.payload[2], 
            i2c_cmd.payload[3], i2c_cmd.payload[4], 
            i2c_cmd.payload[5], i2c_cmd.payload[6]);

    /*************** Open i2c bus *****************************/
    int file_descriptor = open(I2C_BUS_PATH,  O_RDWR);  //g_bus_name[busid].c_str()
    if (file_descriptor  <  0) {
        perror("    ERROR: unable to open this i2c bus\n");
        return RET_FAILED_TO_OPEN_I2C_BUS;
    } else {
        printf("PASS: I2C bus opened, file descriptor: 0x%x \n", file_descriptor);
    }

    /*************** Set I2C slave address *****************************/
    if (ioctl(file_descriptor, I2C_SLAVE, I2C_ADDR_BRIDGE) < 0 ) {
        // if i2c bridge slave address fails
        printf("ERROR: ioctl() File Desc: 0x%x, I2C_SLAVE: 0x%x, I2C_ADDR_BRIDGE: 0x%x\n", \
               file_descriptor, I2C_SLAVE, I2C_ADDR_BRIDGE);
        return RET_I2C_BRIDGE_SLAVE_ADDR_FAILED;
    } else {
        printf("PASS: I2C_SLAVE: 0x%x, I2C_ADDR_Bridge: 0x%02X\n", I2C_SLAVE, I2C_ADDR_BRIDGE);
    }

    /*************** Set Bridge *****************************/
	int num_written = write(file_descriptor, &i2c_cmd.bridge_setting, 1);
	printf("* Wrote %d bytes to BRIDGE: 0x%x\n", num_written, i2c_cmd.bridge_setting);

    /*************** I2C READ *****************************/
    first_read_buf.bridge_address = i2c_cmd.bridge_address * 2;
    first_read_buf.bridge_setting = i2c_cmd.bridge_setting; 
    first_read_buf.response_val = read(file_descriptor, first_read_buf.payload, I2C_READ_LEN_BYTES);
    printf("* 1st read %d bytes %02x %02x %02x %02x\n", 
            first_read_buf.response_val, first_read_buf.payload[0], first_read_buf.payload[1], 
            first_read_buf.payload[2], first_read_buf.payload[3]);

    /*************** I2C WRITE *****************************/
    write_buf.bridge_address *= 2;
    write_buf.bridge_setting = i2c_cmd.bridge_setting;
    write_buf.response_val = write(file_descriptor, &write_buf.payload, write_buf.payload_length);
    usleep(50000);
    printf("* Tried %d bytes, actually wrote %d bytes: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x \n", 
            write_buf.payload_length, write_buf.response_val,
            write_buf.payload[0], write_buf.payload[1],
            write_buf.payload[2], write_buf.payload[3], 
            write_buf.payload[4], write_buf.payload[5]);

    /*************** I2C READ *****************************/
    second_read_buf.bridge_address = i2c_cmd.bridge_address * 2;
    second_read_buf.bridge_setting = i2c_cmd.bridge_setting; 
    second_read_buf.response_val = read(file_descriptor, second_read_buf.payload, I2C_READ_LEN_BYTES);
    printf("* 2nd read %d bytes %02x %02x %02x %02x\n", second_read_buf.response_val, 
            second_read_buf.payload[0], second_read_buf.payload[1], 
            second_read_buf.payload[2], second_read_buf.payload[3]);
    usleep(20000);   //20ms, after DAC WR, immediate RD,  inconsistency between volatile vs EEPROM

    return RET_SUCCESS;
}

/******************************************************************************
 * parse_cmd_line() 
 * 
 * Expected format: <bridge addr> <bridge setting> <cmd> <0+ bytes payload> 
 * Recv: argc: int number of arguments
 *       argv: array of chars containing the arguments
 * Rets: fcn_ret_val indicating success/failure of execution
 *****************************************************************************/
fcn_ret_val parse_cmd_line(int argc, char * argv[]) {
    uint8_t cmd_result = 0;
    struct i2c_cmd_frame i2c_cmd;

    printf("Parsing command line...\n");
    if (1 < argc) {
        i2c_cmd.bridge_address = I2C_ADDR_BRIDGE;
        i2c_cmd.bridge_setting = strtoul(argv[1], NULL, 16);
        i2c_cmd.payload_length = argc - 1;

        // Fill payload buffer with remaining CLI args
        // 1-byte address is the first byte in the buffer.
        // 1-byte command is the second byte in buffer.
        for (int idx = 1; idx < argc; idx++) {
            i2c_cmd.payload[idx-1] = strtoul(argv[idx], NULL, 16);
            i2c_cmd.payload_length++;
        }
        return invoke_cmd(i2c_cmd);

    } else {
        printf("Usage: i2ccmd <bridge setting> <i2c addr> <command> <payload>\n\n");
        return RET_SUCCESS;
    }
}

/******************************************************************************
 * main() 
 * 
 * Recv: argc: int number of arguments
 *       argv: array of chars containing the arguments
 * Rets: int, no meaning
 *****************************************************************************/
int main(int argc, char *argv[]) {
    printf("\nI2C Utility v. %d.%d\n", VERSION_MAJOR, VERSION_MINOR);

    // Parse and execute command
    parse_cmd_line(argc, argv);

	return 0;
}

