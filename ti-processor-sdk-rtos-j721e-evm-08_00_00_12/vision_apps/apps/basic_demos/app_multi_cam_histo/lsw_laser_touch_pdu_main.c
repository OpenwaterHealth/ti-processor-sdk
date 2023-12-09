/*
	Interface between TDA4 and DiCon "VX Stepper Motor Switch Module"

	HW:  I2C bus, labelled as "I2C3",  actual "/dev/i2c-6" channel, upto 400kbps
	           slave device 7-bit address 0x73
	
	SW:  i2c-tools from github opensource base 
	         Open Water customized integration

	 Document:  DiCon VX Stepper Motor Switch MOdule Operation Manual
	                          I2C payload protocol in section 5.7

	  Date:  2021.09.30  leoh@openwater.cc

	 How to build this separated system app on TDA4 Linux user space:
             - scp this file to a TDA4 directory, e.g. /opt/vision_apps
             - cd to that directory,  gcc -o ./lsw_20220404_app  lsw_laser_touch_pdu_main.c -l pthread
             - then the program can be run with ./lsw_20220404_app
		
*/

#include <sys/ioctl.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
//#include <i2c/smbus.h>
//#include "i2cbusses.h"
//#include "util.h"
#include <stdbool.h>  //error: unknown type name �bool�;
#include <fcntl.h>  // error: �O_RDWR� undeclared 
#include <stdint.h>

#include <pthread.h>
//#include <queue>     // does TI  support c++ lib?
#include <ctype.h>   //warning: implicit declaration of function �tolower�

#include <limits.h>
#include <asm/ioctls.h>
#include <asm/termbits.h>    

#include <sys/time.h>    //error: storage size of �itv� isn�t known  struct itimerval  itv;
#include <signal.h>    //error: �SIGALRM� undeclared (first use in this function)

/*   Laser System has seed laser,  transmission amplifier
*/
typedef enum dac_cmd{
	DAC_CMD_WR_DACREG,        // these are actual codes per Table6-2
	DAC_CMD_WR_DACREG1,
	DAC_CMD_WR_RAM,
	DAC_CMD_WR_ALL,
	DAC_CMD_WR_VOLATILECFG_ONLY,
	DAC_CMD_R5,
	DAC_CMD_R6,
	DAC_CMD_R7,
	DAC_CMD_READ = 100,   //  these are not actual codes
	DAC_CMD_GC_RESET,
	DAC_CMD_GC_WAKEUP
}dac_cmd_e;
typedef enum dac_cfg_power_down {
	DAC_CFG_POWERON = 0,
	DAC_CFG_PD_1K,
	DAC_CFG_PD_100K,
	DAC_CFG_PD_500K,
	DAC_CFG_PD_TOTAL
}dac_pd_e;

typedef enum dac_cfg_ref{
	DAC_CFG_VREF_UNBUFFERED_VDD,
	DAC_CFG_VREF_VDD,
	DAC_CFG_VREF_UNBUFFERED_REF,
	DAC_CFG_VREF_BUFFERED_REF
}dac_ref_e;

typedef enum dac_cfg_gain{
	DAC_CFG_GAIN_1X,
	DAC_CFG_GAIN_2X
}dac_gain_e;

typedef enum dac_status_op{
	DAC_STATUS_BUSY,
	DAC_STATUS_READY
}dac_status_op_e;

typedef enum dac_status_por{
	DAC_STATUS_PWROFF,
	DAC_STATUS_PWRON
}dac_status_por_e;


/*
root@j7-evm:/opt/vision_apps/my# gcc -o lsw util.c smbus.c i2cbusses.c  laserswitch_main.c
ly@P52:~/ws/leoh-i2c-tools/tools/my$ gcc -o i2cset util.c smbus.c i2cbusses.c  laserswitch_main.c

*/

/*  Laser Switch Interface at TDA4 SoC side is written in classic C99 on purpose, instead of modern c++, 
    in order to support TI SDK and build configuraiton
*/
typedef enum lswcmds{
        LSW_POLLING_STATUS = 0X30,
        LSW_GET_DEV_INFO, 
        LSW_GET_FW_VER,
        LSW_GET_SN,
        LSW_GET_FW_PART_NO = 0X35,
        LSW_GET_HW_PART_NO,
        LSW_SET_I2C_ADDR,
        LSW_RESET,
        LSW_GET_DEV_DIMENSION = 0X70,
        LSW_SET_IN_OUT = 0X76,
        LSW_GET_OUT_FOR_IN,
        LSW_SET_OUT_FOR_CH1,
        LSW_GET_OUT_FOR_CH1
}lswcmds_e;

// laser switch's commands for user (app) to use, same sequence as following vcmds list
// one cmd needs two params, 3 commands need one param, as stated in below comment line
typedef enum usercmd{
	UCMD_POLLING_STATUS,
	UCMD_GET_DEV_INFO,
	UCMD_GET_FW_VER,
	UCMD_GET_SN,
	UCMD_GET_FW_PART_NO,
	UCMD_GET_HW_PRT_NO,
	UCMD_GET_DEV_DIMENSION,
	UCMD_SET_IN_OUT,			/*param1:  input channel number, 	param2: output ch#*/
	UCMD_GET_OUT_FOR_IN,		/* param1: input channel number*/
	UCMD_SET_OUT_FOR_CH1,	/* param1: output channel number*/
	UCMD_GET_OUT_FOR_CH1,
	UCMD_SET_I2C_ADDR,		/* param1: 7-bit address*/
	UCMD_RESET,
	UCMD_TOTAL_NUM
}lsw_usercmd_e;

#define STR_ENUM(x) {#x, (lswcmds_e)x}
typedef struct  lsw_name_vaue {
	char 		name[24];
	lswcmds_e 	value;
}lsw_string_enum_t;

static lsw_string_enum_t  vcmds[UCMD_TOTAL_NUM] = {  // !!! same sequence as above usercmd enum
        STR_ENUM(LSW_POLLING_STATUS),		//0x30
        STR_ENUM(LSW_GET_DEV_INFO), 		//0x31
        STR_ENUM(LSW_GET_FW_VER),			//0x32
        STR_ENUM(LSW_GET_SN),				//0x33
        STR_ENUM(LSW_GET_FW_PART_NO),		//0x35
        STR_ENUM(LSW_GET_HW_PART_NO),		//0x36
        STR_ENUM(LSW_GET_DEV_DIMENSION),	//0x70
        STR_ENUM(LSW_SET_IN_OUT),			//0x76
        STR_ENUM(LSW_GET_OUT_FOR_IN),		//0x77
        STR_ENUM(LSW_SET_OUT_FOR_CH1),	//0x78
        STR_ENUM(LSW_GET_OUT_FOR_CH1),       //0x79

        STR_ENUM(LSW_SET_I2C_ADDR),			//0x37   dedicated to put address change here
        STR_ENUM(LSW_RESET),				//0x38   reset as the last of this round of testings
};



typedef enum lsw_ch{
	LSW_CH_LEFT_TOP,
	LSW_CH_LEFT_MID,
	LSW_CH_LEFT_BOTTOM,
	LSW_CH_RIGHT_TOP,
	LSW_CH_RIGHT_MID,
	LSW_CH_RIGHT_BOTTOM,
	LSW_CH_TOTAL
} lsw_ch_e;

uint8_t  lsw_laser_port[LSW_CH_TOTAL] = { 1, 3, 5, 2, 4, 6};   //actual port number of the switch,  using lsw_ch_e to index its actual port number



struct  channel_map{
	int  dev_ch;   //  1, 2, ...  on physical switch ports
	int  sw_ch;   	//   left.front.near,    left.front.far;         right.temple.near/far;   
};

typedef enum txrx {
        LSW_TX = 1,
        LSW_RX
}lsw_txrx_e;

typedef enum retcode {
    LSW_STATUS_OK = 0,
    LSW_STATUS_INVALID_CMD,
    LSW_STATUS_VOOR,
    LSW_STATUS_CMD_FAIL,
    LSW_STATUS_MAX
}lsw_status_e;

#define I2C_ADDR_LSW 0X73

#define I2C_ADDR_BRIDGE 0X74

#define I2C_ADDR_ADC1	0X4E		//la
#define I2C_ADDR_ADC2	0X4B
#define I2C_ADDR_DAC1	0X62
#define I2C_ADDR_DAC2	0X63

#define LASER_I2C_CLIENTS  4
uint8_t  i2c_addr_lasermodule[LASER_I2C_CLIENTS] = {0X4E, 0X4B, 0X62, 0X63};



unsigned short int CRC16 (const unsigned char *nData, 
            int wLength     // in bytes
            );

#define LSW_CMD_MAX_LENGTH  124  
typedef struct lsw_cmd{
    lswcmds_e command;    //
    unsigned char type;   // 1- TX,  2 -RX
    lsw_status_e  status;
    uint8_t len; //unsigned char len;      // valid length in bytes, for the to-tx or rxed data[] portion
    unsigned char data[LSW_CMD_MAX_LENGTH];   // [cmd + ... + CRC16]
    // only 1 thread to use this structure for tx, another struct for rx, no concurrent access so far
}lsw_cmd_t;

lsw_cmd_t lswtx, lswrx;   //global variables to store the results of below APIs

int fd = -1;
uint8_t  current_dev;  // valid address

typedef enum thread_name{
	P_DISPLAY_TX,
	P_DISPLAY_RX,
	P_LASER_SWITCH,
	P_LASER_SYSTEM,
	P_POWER_SRC,
	P_THREADS_TOTAL
}ow_threads_e;

pthread_t  p_ids[P_THREADS_TOTAL];
bool p_running_flags[P_THREADS_TOTAL];   //global to initialized as 0/false,  false to quit, true to keep running
pthread_mutex_t p_mutex[P_THREADS_TOTAL]; // i2c6 _mutex;  p_mutex[P_LASER_SWITCH]
pthread_cond_t   p_cond[P_THREADS_TOTAL];
bool new_flags[P_THREADS_TOTAL];		//  true to trigger a thread to process its queue or particular global object data

///////////////////////////////////uart.start//////////////////////////////////////////////
typedef enum touchcmd{
	UI_SCREEN1_START = 0X0100,
	UI_SCREEN1_PATIENTID_ENTER,

	UI_SCREEN2_TEST_ALL = 0X0200,
	UI_SCREEN2_SELECTED_SCAN,

	UI_SCREEN3_SCAN_REDO = 0X0300,
	UI_SCREEN3_SCAN_SAVE,

	UI_SCREEN4_SELF_CALIBRATION = 0X0400,
	UI_SCREEN4_RECOVER_DEFAULT

}ui_cmds_e;

typedef struct  tlv_entry{
	uint16_t   type;
	uint16_t   len;
	char      	value[256];
}ow_tlv_t;

ow_tlv_t  rx_obj, tx_obj;   // one by one so far, may need to use a queue in complex system

#define MAX_LOOPBACK_SIZE 256 // MAX_READ_SIZE + CMD_FORMAT_LEN
#define MAX_READ_SIZE 	  256

struct UartDevice {
	char* filename;
	int rate;
	int fd;
	struct termios2 *tty;   // termios replaced
};

int uart_start(struct UartDevice* dev, bool canonic);
int uart_write(struct UartDevice* dev, char *string);
int uart_read(struct UartDevice* dev, char *buf, size_t buf_len);
void uart_stop(struct UartDevice* dev);
#define OW_API
int timeout_ms_  = -1;

	void ConfigureTermios( int fd, unsigned int rate, struct termios2 * tty )
	{

		ioctl(fd, TCGETS2, tty);   //tcgetattr(fd, tty);
		printf(" I N I  c_cflag=%X iflag=%X oflag=%X, lflag=%X,"
				"line=%X, ispeed=%d, ospeed=%d \n", tty->c_cflag, 
				tty->c_iflag, tty->c_oflag, tty->c_lflag,
				tty->c_line, tty->c_ispeed, tty->c_ospeed
				);

		for(int i = 0; i < 19; i++) { printf(" %02x", tty->c_cc[i]);}	printf("\n");

		tty->c_cflag     &=  ~PARENB;       	// No parity bit 
		tty->c_cflag     &=  ~CSTOPB;		// Only one stop-bit is used
		tty->c_cflag     &=  ~CSIZE;		// CSIZE is a mask for the number of bits per character
		tty->c_cflag     |=  CS8;		// Set to 8 bits per character
		tty->c_cflag     &=  ~CRTSCTS;       	// Disable hadrware flow control (RTS/CTS)
		tty->c_cflag     |=  CREAD | CLOCAL;   	// Turn on READ & ignore ctrl lines (CLOCAL = 1)


		tty->c_ispeed = rate;
		tty->c_ospeed = rate;
		tty->c_oflag     =   0;              // No remapping, no delays
		tty->c_oflag     &=  ~OPOST;			// Make raw

		if(timeout_ms_ == -1) {
		    // Always wait for at least one byte, this could block indefinitely
		    tty->c_cc[VTIME] = 0;
		    tty->c_cc[VMIN] = 1;
		} else if(timeout_ms_ == 0) {
		    // Setting both to 0 will give a non-blocking read
		    tty->c_cc[VTIME] = 0;
		    tty->c_cc[VMIN] = 0;
		} else if(timeout_ms_ > 0) {
		    tty->c_cc[VTIME] = (cc_t)(timeout_ms_/100);    // 0.5 seconds read timeout
		    tty->c_cc[VMIN] = 0;
		}


		tty->c_iflag     &= ~(IXON | IXOFF | IXANY);			// Turn off s/w flow ctrl
		tty->c_iflag 	&= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

		tty->c_lflag		&= ~ICANON;		// Turn off canonical input, which is suitable for pass-through
		tty->c_lflag |= ECHO;
		tty->c_lflag		&= ~ECHOE;	
		tty->c_lflag		&= ~ECHONL;		
		tty->c_lflag		&= ~ISIG;	
		printf(" S E T  c_cflag=%X iflag=%X oflag=%X, lflag=%X,"
				"line=%X, ispeed=%d, ospeed=%d \n", tty->c_cflag, 
				tty->c_iflag, tty->c_oflag, tty->c_lflag,
				tty->c_line, tty->c_ispeed, tty->c_ospeed
				);
		for(int i = 0; i < 19; i++) { printf(" %02x", tty->c_cc[i]);} printf("\n");
		tty->c_cflag = 0x1CB0;
		tty->c_lflag = 0x8A20;
		tty->c_ispeed = 115200;
		tty->c_ospeed = 115200;

		ioctl(fd, TCSETS2, tty);   //if(tcsetattr(fd, TCSANOW, tty) != 0){}

		sleep(1);
		
		ioctl(fd, TCGETS2, tty);   //tcgetattr(fd, tty);
		printf(" N E W  c_cflag=%X iflag=%X oflag=%X, lflag=%X,"
				"line=%X, ispeed=%d, ospeed=%d \n", tty->c_cflag, 
				tty->c_iflag, tty->c_oflag, tty->c_lflag,
				tty->c_line, tty->c_ispeed, tty->c_ospeed
				);
		for(int i = 0; i < 19; i++) { printf(" %02x", tty->c_cc[i]);} printf("\n");
	}

/*
 * Start the UART device.
 *
 * @param dev points to the UART device to be started, must have filename and rate populated
 * @param canonical whether to define some compatibility flags for a canonical interface
 *
 * @return - 0 if the starting procedure succeeded
 *         - negative if the starting procedure failed
 */
OW_API int uart_start(struct UartDevice* dev, bool canonical) {
	//struct termios *tty;
	struct termios2 tty;
	int fd;
	int rc;

	fd = open(dev->filename, O_RDWR | O_NOCTTY);
	if (fd < 0) {
		printf("%s: failed to open UART device %s\r\n", __func__, dev->filename);
		return fd;
	}
	ConfigureTermios(fd, 115200, &tty);
	dev->fd = fd;
	dev->tty = &tty;
	return 0;
}

/*
 * Read a string from the UART device.
 *
 * @param dev points to the UART device to be read from
 * @param buf points to the start of buffer to be read into
 * @param buf_len length of the buffer to be read
 *
 * @return - number of bytes read if the read procedure succeeded
 *         - negative if the read procedure failed
 */
OW_API int uart_read(struct UartDevice* dev, char *buf, size_t buf_len) {
	int rc;
	rc = read(dev->fd, buf, buf_len - 1);
	if (rc < 0) {
		printf("%s: failed to read uart data\r\n", __func__);
		return rc;
	}
	buf[rc] = '\0';
	return rc;
}

/*
 * Write a string to the UART device.
 *
 * @param dev points to the UART device to be written to
 * @param string points to the start of buffer to be written from
 *
 * @return - number of bytes written if the write procedure succeeded
 *         - negative if the write procedure failed
 */
OW_API int uart_write(struct UartDevice* dev, char *sp) {
	size_t len = strlen(sp);
	return write(dev->fd, sp, len);
}

/*
 * Stop the UART device.
 *
 * @param dev points to the UART device to be stopped
 */
OW_API void uart_stop(struct UartDevice* dev) {
	free(dev->tty);
}
//////////////////////////////////////uart.end/////////////////////////////////////////////////
#define ADC_LENGTH  (10)     // bits
#define ADC_REF_VOLT (5000)    //mV
#define DAC_LENGTH  (10)     // bits
#define DAC_REF_VOLT (5000)    //mV

#define DAC1_NV_DATA  (512)     // 307 = 0X133 = 0100110011'b   for 0x62 address
#define DAC1_RAM_DATA (300)  // 300 = 0X12C = 01 0010 1100 'b
#define DAC2_NV_DATA  (256)     // 307 = 0X133 = 0100110011'b    for 0x63 address
#define DAC2_RAM_DATA (200)  // 300 = 0X12C = 01 0010 1100 'b

struct dac_object{
	uint8_t  vol_gain:1;
	uint8_t  vol_pd:2;
	uint8_t  vol_ref:2;
	uint8_t  vol_r0:1;
	uint8_t  vol_por:1;
	uint8_t vol_rdy:1;

	uint16_t vol_data;      // only lsb 10 bits are valid

	uint8_t  nv_gain:1;
	uint8_t  nv_pd:2;
	uint8_t  nv_ref:2;
	uint8_t  nv_r1:1;
	uint8_t  nv_por:1;
	uint8_t nv_rdy:1;

	uint16_t nv_data;      // only lsb 10 bits are valid
}dac_read_result;

// input : buf,  WR - will be updated by this function as the 1st result, buf[0] is the I2C device address
//                                   buf[0] address for General Call Reset, General Call Wakeup must be 0x00
//			READ - decode, input 'n' shall be >= 6 bytes, buf[0] is the first data as well instead of I2C address
// input : n,  number of bytes will be I2C txed, will be updated by this function as the 2nd result
// input :  'data'  'cmd' 'ref' 'pd'  'gain'
//                only inputs,  not each one is required, depends on cmd type


OW_API int mcp4716_dac_cmd_codec(uint8_t *buf,
	uint8_t *n,
	uint16_t data, dac_cmd_e cmd, dac_ref_e ref, dac_pd_e pd, dac_gain_e gain){
	int ret = 0;
	if(cmd == DAC_CMD_WR_DACREG) {
		buf[1] = (DAC_CMD_WR_DACREG << 6)  | (pd << 4) |(data >> 6);
		buf[2] = (data & 0x3F) << 2;
		printf("  DAC_CMD_WR_DACREG WR buf %02x  %02x \n", buf[1], buf[2]);
		*n = 2;
	} else if(cmd == DAC_CMD_WR_RAM){
		buf[1] = (DAC_CMD_WR_RAM << 5) | (ref << 3) | (pd << 1) | gain;
		buf[2] = (data >> 2);
		buf[3] = (data & 0x03) << 6;
		printf("  DAC_CMD_WR_RAM WR buf %02x  %02x  %0x2\n", buf[1], buf[2], buf[3]);
		*n = 3;
	} else if(cmd == DAC_CMD_WR_ALL){
		buf[1] = (DAC_CMD_WR_ALL << 5) | (ref << 3) | (pd << 1) | gain;
		buf[2] = (data >> 2);
		buf[3] = (data & 0x03) << 6;
		printf("  DAC_CMD_WR_ALL WR buf %02x  %02x  %02x\n", buf[1], buf[2], buf[3]);
		*n = 3;
	} else if(cmd == DAC_CMD_WR_VOLATILECFG_ONLY){
		buf[1] = (DAC_CMD_WR_VOLATILECFG_ONLY << 5) | (ref << 3) | (pd << 1) | gain;
		printf("  DAC_CMD_WR_VOLATILECFG_ONLY WR buf %02x \n", buf[1]);
		*n = 1;
	} else if(cmd == DAC_CMD_GC_RESET){
		buf[0] = 0x00;
		buf[1] = 0x06;   // all DACs on this bus, will be reset synchronously
		printf("  WR buf %02x \n", buf[1]);
		*n = 1;
	} else if(cmd == DAC_CMD_GC_WAKEUP){
		buf[0] = 0x00;
		buf[1] = 0x09;   // all DACs on this bus, will be waken up synchronously
		printf("  WR buf %02x \n", buf[1]);
		*n = 1;
	} else if(cmd == DAC_CMD_READ){     // to decode
		int bytes = read(fd, buf, 12);
		printf("DAC_CMD_READ laser DAC-check read %d bytes\n", bytes);

		dac_read_result.vol_gain = buf[0] & 0x01;
		dac_read_result.vol_pd =(buf[0] & 0x06) >> 1;
		dac_read_result.vol_ref =(buf[0] & 0x18) >> 3;
		dac_read_result.vol_por=(buf[0] & 0x40) >> 6;
		dac_read_result.vol_rdy =(buf[0] & 0x80) >> 7;
		dac_read_result.vol_data = buf[1];
		dac_read_result.vol_data <<= 2;
		dac_read_result.vol_data += (buf[2] >> 6);

		dac_read_result.nv_gain = buf[3] & 0x01;
		dac_read_result.nv_pd =(buf[3] & 0x06) >> 1;
		dac_read_result.nv_ref =(buf[3] & 0x18) >> 3;
		dac_read_result.nv_por=(buf[3] & 0x40) >> 6;
		dac_read_result.nv_rdy =(buf[3] & 0x80) >> 7;
		dac_read_result.nv_data = buf[4];
		dac_read_result.nv_data <<= 2;
		dac_read_result.nv_data += (buf[5] >> 6);

		printf("RAW read data= "); for(int k = 0; k < 12; k++) printf(" %02x ", buf[k]); printf("\n");

		printf("  READ decoded buf  vola memo gain=%d pd=%d ref=%d por=%d rdy=%d data=%d %d mV\n", dac_read_result.vol_gain,
					dac_read_result.vol_pd, dac_read_result.vol_ref, dac_read_result.vol_por, dac_read_result.vol_rdy,
					dac_read_result.vol_data, dac_read_result.vol_data * DAC_REF_VOLT/(1 << DAC_LENGTH));
		printf("  READ decoded buf  EEPROM NV gain=%d pd=%d ref=%d por=%d rdy=%d data=%d %d mV\n", dac_read_result.nv_gain,
					dac_read_result.nv_pd, dac_read_result.nv_ref, dac_read_result.nv_por, dac_read_result.nv_rdy,
					dac_read_result.nv_data, dac_read_result.nv_data * DAC_REF_VOLT/(1 << DAC_LENGTH));

		*n = 6;
	} else {
		ret = -1;
	}
	if ( cmd >= DAC_CMD_WR_DACREG && cmd <= DAC_CMD_WR_VOLATILECFG_ONLY) {
		int bytes = write(fd, &buf[1], *n);
		usleep(100000);   // 50ms origin
		printf(" wrote %d bytes ... executed cmd=%d  pd=%d\n",	bytes,  cmd,  pd);
	}

	return ret;

}

int laser_init(){
		if (fd < 0) {
			fd = open("/dev/i2c-6",  O_RDWR);  //g_bus_name[busid].c_str()

			if(fd  <  0) {
				perror(" unable to open this i2c bus\n");
				return (-1);
			}
		}
		printf(" the fd for i2c6 is %d\n", fd);

		if ( ioctl(fd, I2C_SLAVE, I2C_ADDR_BRIDGE) < 0 ){
			perror(" io ctl  i2c BRIDGE slave address failed\n");
			return (-1);
		}else{
			printf("\n io ctl  i2c BRIDGE slave address ok ... %02X\n", I2C_ADDR_BRIDGE);
			uint8_t buf[16], len;
			buf[0] = I2C_ADDR_BRIDGE*2;
			buf[1] = 0x10;

			int n = write(fd, &buf[1], 1);
			printf(" wrote %d bytes to BRIDGE\n", n);
			buf[0] = 0;  buf[1] = 0; // clear up
			n = read (fd, buf, 4);
			printf("BRIDGE read %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3]);

			for (int j = 0; j < 4; j++){
				memset(buf, 0, 16);

				if ( ioctl(fd, I2C_SLAVE, i2c_addr_lasermodule[j]) < 0 ){
					perror(" io ctl  i2c  laser-seed-check slave address failed\n");
					return (-1);
				}
				printf("\n io ctl  i2c laser-seed-check slave address ok ... %02X\n", i2c_addr_lasermodule[j]);
				if(i2c_addr_lasermodule[j] == 0x62){
					mcp4716_dac_cmd_codec(buf, &len, 309, DAC_CMD_WR_ALL, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);

					mcp4716_dac_cmd_codec(buf, &len, DAC1_NV_DATA, DAC_CMD_WR_DACREG, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);

					n = write(fd, &buf[1], len);   // 2
					printf(" wrote %d bytes to laser-seed DAC1 DAC Register  v=%d\n", n, DAC1_NV_DATA);

					mcp4716_dac_cmd_codec(buf, &len, DAC1_RAM_DATA, DAC_CMD_WR_RAM, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);
					n = write(fd, &buf[1], len); /*3 bytes of data*/
					printf(" wrote %d bytes to laser-seed DAC1  volatile RAM  v=%d\n", n, DAC1_RAM_DATA);

				}else if (i2c_addr_lasermodule[j] == 0x63) {
					mcp4716_dac_cmd_codec(buf, &len, 209, DAC_CMD_WR_ALL, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);  //section 6.3
					n = write(fd, &buf[1], len);
					printf(" wrote %d bytes to laser-seed DAC2 DAC ALL  v=%d\n", n, 209);
					usleep(50000);


					mcp4716_dac_cmd_codec(buf, &len, /*DAC2_NV_DATA*/1023, DAC_CMD_WR_DACREG, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X); // section 6.1
					n = write(fd, &buf[1], len);
					printf(" wrote %d bytes to laser-seed DAC2 DAC Register  v=%d\n", n, DAC2_NV_DATA);

					mcp4716_dac_cmd_codec(buf, &len, /*DAC2_RAM_DATA*/320, DAC_CMD_WR_RAM, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);  //section 6.2
					n = write(fd, &buf[1], 3);
					printf(" wrote %d bytes to laser-seed DAC2  volatile RAM  v=%d\n", n, DAC2_RAM_DATA);
				}
				memset(buf, 0, 16);

				n = read (fd, buf, 12);

				if (i2c_addr_lasermodule[j] == 0x4E || i2c_addr_lasermodule[j] == 0x4b){
					printf("laser-seed ADC-check read %d bytes  %02x %02x %02x %02x %02x %02x ... %d mV\n", n,
						buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], (((buf[0] << 8) + buf[1]) >> 2)*ADC_REF_VOLT/(1 << ADC_LENGTH));
				}else if (i2c_addr_lasermodule[j] == 0x62 || i2c_addr_lasermodule[j] == 0x63){
					printf("laser-seed DAC-check read\n");
					mcp4716_dac_cmd_codec(buf,  (uint8_t*)&n, 0, DAC_CMD_READ, DAC_CFG_VREF_UNBUFFERED_VDD,DAC_CFG_POWERON,DAC_CFG_GAIN_1X);
					usleep(20000);   //20ms, after DAC WR, immediate RD,  inconsistency between volatile vs EEPROM
					//mcp4716_dac_cmd_codec(buf,  &n, 0, DAC_CMD_READ, 0,0,0);
				}
			}

			  if ( ioctl(fd, I2C_SLAVE, I2C_ADDR_BRIDGE) < 0 ){
					perror(" io ctl  i2c BRIDGE slave address failed\n");
					return (-1);
			  }else{
				printf("\n io ctl  i2c BRIDGE slave address ok ... %02X\n", I2C_ADDR_BRIDGE);
				//char buf[8];
				buf[0] = I2C_ADDR_BRIDGE*2; buf[1] = 0x20;

				n = write(fd, &buf[1], 1);
				printf(" wrote %d bytes to BRIDGE\n", n);
				buf[0] = 0;  buf[1] = 0; // clear up
				n = read (fd, buf, 4);
				printf("BRIDGE read %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3]);

				for (int j = 0; j < 4; j++){
					memset(buf, 0, 16);

					if ( ioctl(fd, I2C_SLAVE, i2c_addr_lasermodule[j]) < 0 ){
						perror(" io ctl  i2c  laser-TA-check slave address failed\n");
						return (-1);
					}
					printf("\n io ctl  i2c laser-TA-check slave address ok ... %02X\n", i2c_addr_lasermodule[j]);

					if(i2c_addr_lasermodule[j] == 0x62){
						buf[1] = (DAC_CMD_WR_DACREG << 6)  | (DAC_CFG_POWERON << 4) |(DAC1_NV_DATA >> 6);
						buf[2] = (DAC1_NV_DATA & 0xCF) << 2;
						printf("  WR buf %02x  %02x \n", buf[1], buf[2]);
						n = write(fd, &buf[1], 2);
						printf(" wrote %d bytes to laser-TA DAC1 DAC Register  v=%d\n", n, DAC1_NV_DATA);
					}
					memset(buf, 0, 16);

					n = read (fd, buf, 14);

					if (i2c_addr_lasermodule[j] == 0x4E || i2c_addr_lasermodule[j] == 0x4b){
						printf("laser-TA ADC-check read %d bytes  %02x %02x %02x %02x %02x %02x ... %d mV\n", n,
							buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], (((buf[0] << 8) + buf[1]) >> 2)*ADC_REF_VOLT/(1 << ADC_LENGTH));
					}else if (i2c_addr_lasermodule[j] == 0x62 || i2c_addr_lasermodule[j] == 0x63){
						printf("laser-TA DAC-check read\n");
						mcp4716_dac_cmd_codec(buf,  (uint8_t*)&n, 0, DAC_CMD_READ,  DAC_CFG_VREF_UNBUFFERED_VDD,DAC_CFG_POWERON,DAC_CFG_GAIN_1X);
						usleep(20000);   //20ms, after DAC WR, immediate RD,  inconsistency between volatile vs EEPROM
						//mcp4716_dac_cmd_codec(buf,  &n, 0, DAC_CMD_READ, 0,0,0);
					}
				}
			   }
		}


		return 0;
}

// initialize the I2C bus on which laser switch is connected 
// return : 0 successful, -1 failed
int lsw_api_init(){
		if (fd < 0) { 
			fd = open("/dev/i2c-6",  O_RDWR);  //g_bus_name[busid].c_str()
			
			if(fd  <  0) {
				perror(" unable to open this i2c bus\n");
				return (-1); 
			}
		}
		printf(" the fd for i2c6 is %d\n", fd);

		if ( ioctl(fd, I2C_SLAVE, I2C_ADDR_BRIDGE) < 0 ){
			perror(" io ctl  i2c BRIDGE slave address failed\n");
			return (-1);
		}else{
			printf("\n io ctl  i2c BRIDGE slave address ok ... %02X\n", I2C_ADDR_BRIDGE);
			uint8_t buf[16], len;
			uint16_t datav, dv2;
			buf[0] = I2C_ADDR_BRIDGE*2; buf[1] = 0x10;
			
			int n = write(fd, &buf[1], 1);
			printf(" wrote %d bytes to BRIDGE\n", n);
			buf[0] = 0;  buf[1] = 0; // clear up
			n = read (fd, buf, 4);
			printf("BRIDGE read %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3]);

			for (int j = 0; j < 4; j++){
				memset(buf, 0, 16);

				if ( ioctl(fd, I2C_SLAVE, i2c_addr_lasermodule[j]) < 0 ){
					perror(" io ctl  i2c  laser-seed-check slave address failed\n");
					return (-1);
				}
				printf("\n io ctl  i2c laser-seed-check slave address ok ... %02X\n", i2c_addr_lasermodule[j]);
				if(i2c_addr_lasermodule[j] == 0x62){
					mcp4716_dac_cmd_codec(buf, &len, 309, DAC_CMD_WR_ALL, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);
					n = write(fd, &buf[1], len);
					printf(" wrote %d bytes to laser-seed DAC1 DAC ALL  v=%d\n", n, 309);
					usleep(50000);

					mcp4716_dac_cmd_codec(buf, &len, DAC1_NV_DATA, DAC_CMD_WR_DACREG, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);

					n = write(fd, &buf[1], len);   // 2
					printf(" wrote %d bytes to laser-seed DAC1 DAC Register  v=%d\n", n, DAC1_NV_DATA);


					mcp4716_dac_cmd_codec(buf, &len, DAC1_RAM_DATA, DAC_CMD_WR_RAM, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);
					n = write(fd, &buf[1], len); /*3 bytes of data*/
					printf(" wrote %d bytes to laser-seed DAC1  volatile RAM  v=%d\n", n, DAC1_RAM_DATA);

				}else if (i2c_addr_lasermodule[j] == 0x63) {
					mcp4716_dac_cmd_codec(buf, &len, 209, DAC_CMD_WR_ALL, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);  //section 6.3
					n = write(fd, &buf[1], len);
					printf(" wrote %d bytes to laser-seed DAC2 DAC ALL  v=%d\n", n, 209);
					usleep(50000);


					mcp4716_dac_cmd_codec(buf, &len, /*DAC2_NV_DATA*/1023, DAC_CMD_WR_DACREG,DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X); // section 6.1
					n = write(fd, &buf[1], len);
					printf(" wrote %d bytes to laser-seed DAC2 DAC Register  v=%d\n", n, DAC2_NV_DATA);


					mcp4716_dac_cmd_codec(buf, &len, /*DAC2_RAM_DATA*/320, DAC_CMD_WR_RAM, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);  //section 6.2
					n = write(fd, &buf[1], 3);
					printf(" wrote %d bytes to laser-seed DAC2  volatile RAM  v=%d\n", n, DAC2_RAM_DATA);
				}
				memset(buf, 0, 16);

				n = read (fd, buf, 12);

				if (i2c_addr_lasermodule[j] == 0x4E || i2c_addr_lasermodule[j] == 0x4b){
					printf("laser-seed ADC-check read %d bytes  %02x %02x %02x %02x %02x %02x ... %d mV\n", n, 
						buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], (((buf[0] << 8) + buf[1]) >> 2)*ADC_REF_VOLT/(1 << ADC_LENGTH));
				}else if (i2c_addr_lasermodule[j] == 0x62 || i2c_addr_lasermodule[j] == 0x63){
					printf("laser-seed DAC-check read\n");
					mcp4716_dac_cmd_codec(buf,  (uint8_t*)&n, 0, DAC_CMD_READ, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);
					usleep(20000);   //20ms, after DAC WR, immediate RD,  inconsistency between volatile vs EEPROM
					//mcp4716_dac_cmd_codec(buf,  &n, 0, DAC_CMD_READ, 0,0,0);
				#if 0
				        datav = buf[1];
					datav <<= 2;
					datav += (buf[2] >> 6);
				        dv2 = buf[4];
					dv2 <<= 2;
					dv2 += (buf[5] >> 6);

					printf("laser-seed DAC-check read %d bytes  %02x %02x %02x %02x %02x %02x %02x ... DAC-value: volatile %d  %d mV NV %d %d mV \n", 
						n,
						buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6],
						/*(((buf[1] << 2) + buf[2]) >> 6), (((buf[1] << 2) + buf[2]) >> 6)*DAC_REF_VOLT/(1 << DAC_LENGTH),
						(((buf[4] << 2) + buf[5]) >> 6), (((buf[4] << 2) + buf[5]) >> 6)*DAC_REF_VOLT/(1 << DAC_LENGTH)*/
						datav, datav * DAC_REF_VOLT/(1 << DAC_LENGTH),
						dv2, dv2 * DAC_REF_VOLT/(1 << DAC_LENGTH)
						);
				#endif
				}
			}

			  if ( ioctl(fd, I2C_SLAVE, I2C_ADDR_BRIDGE) < 0 ){
					perror(" io ctl  i2c BRIDGE slave address failed\n");
					return (-1);
			  }else{
				printf("\n io ctl  i2c BRIDGE slave address ok ... %02X\n", I2C_ADDR_BRIDGE);
				//char buf[8];
				buf[0] = I2C_ADDR_BRIDGE*2; buf[1] = 0x20;

				n = write(fd, &buf[1], 1);
				printf(" wrote %d bytes to BRIDGE\n", n);
				buf[0] = 0;  buf[1] = 0; // clear up
				n = read (fd, buf, 4);
				printf("BRIDGE read %02x %02x %02x %02x\n", buf[0], buf[1], buf[2], buf[3]);

				for (int j = 0; j < 4; j++){
					memset(buf, 0, 16);

					if ( ioctl(fd, I2C_SLAVE, i2c_addr_lasermodule[j]) < 0 ){
						perror(" io ctl  i2c  laser-TA-check slave address failed\n");
						return (-1);
					}
					printf("\n io ctl  i2c laser-TA-check slave address ok ... %02X\n", i2c_addr_lasermodule[j]);

					if(i2c_addr_lasermodule[j] == 0x62){
						buf[1] = (DAC_CMD_WR_DACREG << 6)  | (DAC_CFG_POWERON << 4) |(DAC1_NV_DATA >> 6);
						buf[2] = (DAC1_NV_DATA & 0xCF) << 2;
						printf("  WR buf %02x  %02x \n", buf[1], buf[2]);
						n = write(fd, &buf[1], 2);
						printf(" wrote %d bytes to laser-TA DAC1 DAC Register  v=%d\n", n, DAC1_NV_DATA);

						/*buf[1] = (DAC_CMD_WR_RAM << 5) | (DAC_CFG_VREF_UNBUFFERED_VDD << 3) | (DAC_CFG_POWERON << 1) |DAC_CFG_GAIN_1X;
						buf[2] = (DAC1_RAM_DATA >> 2);
						buf[3] = (DAC1_RAM_DATA & 0x03) << 6;
						printf("  WR buf %02x  %02x  %0x2\n", buf[1], buf[2], buf[3]);
						n = write(fd, &buf[1], 3);
						printf(" wrote %d bytes to laser-TA DAC1  volatile RAM  v=%d\n", n, DAC1_RAM_DATA);
						TC result:  EEPROM-WR first, volatile memory WR later, volatile value wins */
					}
					memset(buf, 0, 16);

					n = read (fd, buf, 14);

					if (i2c_addr_lasermodule[j] == 0x4E || i2c_addr_lasermodule[j] == 0x4b){
						printf("laser-TA ADC-check read %d bytes  %02x %02x %02x %02x %02x %02x ... %d mV\n", n,
							buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], (((buf[0] << 8) + buf[1]) >> 2)*ADC_REF_VOLT/(1 << ADC_LENGTH));
					}else if (i2c_addr_lasermodule[j] == 0x62 || i2c_addr_lasermodule[j] == 0x63){
						printf("laser-TA DAC-check read\n");
						mcp4716_dac_cmd_codec(buf,  (uint8_t*)&n, 0, DAC_CMD_READ, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);
						usleep(20000);   //20ms, after DAC WR, immediate RD,  inconsistency between volatile vs EEPROM
						//mcp4716_dac_cmd_codec(buf,  &n, 0, DAC_CMD_READ, 0,0,0);
						#if 0
					        datav = buf[1];
						datav <<= 2;
						datav += (buf[2] >> 6);
					        dv2 = buf[4];
						dv2 <<= 2;
						dv2 += (buf[5] >> 6);

						printf("laser-TA DAC-check read %d bytes  %02x %02x %02x %02x %02x %02x %02x ... DAC-value: volatile %d  %d mV NV %d %d mV \n", 
							n,
							buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], buf[6],
							/*(((buf[1] << 2) + buf[2]) >> 6), (((buf[1] << 2) + buf[2]) >> 6)*DAC_REF_VOLT/(1 << DAC_LENGTH),
							(((buf[4] << 2) + buf[5]) >> 6), (((buf[4] << 2) + buf[5]) >> 6)*DAC_REF_VOLT/(1 << DAC_LENGTH)*/
							datav, datav * DAC_REF_VOLT/(1 << DAC_LENGTH),
							dv2, dv2 * DAC_REF_VOLT/(1 << DAC_LENGTH)
							);
						#endif
					}
				}
			   }
		}

		if ( ioctl(fd, I2C_SLAVE, I2C_ADDR_LSW) < 0 ){
			//printf(" io ctl  i2c slave address failed\n");
			perror(" io ctl  LaserSwitch i2c slave address failed\n");
			return (-1);
		}else{
			printf("\n io ctl  LaserSwitch i2c slave address ok ... %02X\n", I2C_ADDR_LSW);
			current_dev = I2C_ADDR_LSW;
		}
		return 0;
}

int i2c_addr_select(uint8_t addr){
		if (fd < 0) { 
			fd = open("/dev/i2c-6",  O_RDWR);  //g_bus_name[busid].c_str()
			
			if(fd  <  0) {
				perror(" unable to open this i2c bus\n");
				return (-1); 
			}
			printf(" the fd for i2c6 is %d\n", fd);
		}
		//printf(" the fd for i2c6 is %d\n", fd);
		if ( ioctl(fd, I2C_SLAVE, addr) < 0 ){
			//printf(" io ctl  i2c slave address failed\n");
			char errstr[64];
			snprintf(errstr, 64, " io ctl  generic i2c slave address 0x%02X failed", addr);
			perror(errstr);
			return (-1);
		}else{
			printf(" io ctl  generic i2c slave address 0x%02X ok\n", addr);
			current_dev = addr;
		}
		return 0;
}

// channel=0x10, channel 4 for Laser Seed; 0x20 channel 5 for TA
int i2c_switch_channel(uint8_t channel){

		uint8_t buf[16], len;
		buf[0] = I2C_ADDR_BRIDGE*2;
		buf[1] = channel;
		i2c_addr_select(I2C_ADDR_BRIDGE);
		int n = write(fd, &buf[1], 1);
		buf[0] = 0;  buf[1] = 0; // clear up
		n = read(fd, buf, 2);
		printf("BRIDGE channel read %02x %02x\n", buf[0], buf[1]);
		return 0;
}


int lsw_api_stop(){
		return 0;
}

// output is stored in 'lswtx' struct, lswtx.data[] byte array will be put on I2C
// depending on I2C API function, the first byte 7-bit 'ADDRESS' shifted to MSB position + 1 bit R/W flag, 
//                                shall or shall not be included !!! 
void lsw_tx_prepare(lsw_string_enum_t  * cmd, //lswcmds_e cmd,          // IN
                        //lsw_txrx_e type,      // IN
                        uint8_t param1,   // optional IN, depends on cmd, mandatory for group 2,3 below
                        uint8_t param2    // optional IN, depends on cmd, mandatory for group 3
                        ){
    unsigned int idx = 0;
    unsigned short int crc16;
    lswtx.command = cmd->value;
    //if (type == LSW_TX){
        lswtx.data[idx++] = I2C_ADDR_LSW * 2;
        lswtx.type = LSW_TX;
        switch ( cmd->value) {                         //no param needed for such cmd ... group 1
            case LSW_POLLING_STATUS:
            case LSW_GET_DEV_INFO:
            case LSW_GET_FW_VER:
            case LSW_GET_SN:
            case LSW_GET_FW_PART_NO:
            case LSW_GET_HW_PART_NO:
            case LSW_RESET:
            case LSW_GET_DEV_DIMENSION:
            case LSW_GET_OUT_FOR_CH1:
                lswtx.data[idx++] = cmd->value;
                break;

            case LSW_SET_I2C_ADDR:              //one param is needed for such cmd ... group 2
            case LSW_GET_OUT_FOR_IN:
            case LSW_SET_OUT_FOR_CH1:
                lswtx.data[idx++] = cmd->value;
                lswtx.data[idx++] = param1;
                break;

            case LSW_SET_IN_OUT:                //two params are needed for such cmd ... group 3
                lswtx.data[idx++] = cmd->value;
                lswtx.data[idx++] = param1;
                lswtx.data[idx++] = param2;
                break;
       
            default:
                printf(" error this cmdid = %02X is not supported  !", cmd->value);
                lswtx.status = LSW_STATUS_INVALID_CMD;
                return;
        }

        crc16 = CRC16(lswtx.data, idx);
        printf("%02X:%04X -----------------------write %s \n\n", cmd->value, crc16, cmd->name);
        // big endian of the CRC part; little endian CRC didn't work.
        lswtx.data[idx++] = (unsigned char)(crc16 >> 8); //(unsigned char)(crc16 & 0xFF);
        lswtx.data[idx++] = (unsigned char)(crc16 & 0xFF); //(unsigned char)(crc16 >> 8);
        lswtx.len = idx;
        lswtx.status = LSW_STATUS_OK;
    //}else{
    //    lswtx.status = LSW_STATUS_INVALID_CMD;
    //}
}


//return:  true/CRC16 pass (record inside 'lswrx' object),  false/CRC16 failed (discard)
bool lsw_rx_check(lswcmds_e cmd,        // IN
                unsigned char * dataptr,    // IN rxed i2c data bytes without address, with CRC16
                unsigned int leng){        // IN in bytes
    unsigned int idx = 0;
    unsigned short int crc16;
    if (dataptr == NULL || leng < 3) return false;
    if (((unsigned char)cmd) != (*dataptr) ) return false;    // cmd type must be matching

    lswrx.command = cmd;
    lswrx.type = LSW_RX;

    lswrx.data[idx++] = I2C_ADDR_LSW*2 + 1;
    for(int i = 0; i < leng; ){   // CRC16 is copied, but not used by re-calculation
        lswrx.data[idx++] = dataptr[i++];
    }
    printf("%02X:%02X:%02X ---packet ---%d valid bytes  \n\n", lswrx.data[0], lswrx.data[1], lswrx.data[2],  idx - 2 );
    crc16 = CRC16(lswrx.data, idx - 2);     //exclude the last 2  bytes of read-CRC16
    //crc16  = ((crc16&0xFF)<< 8) +   ((crc16&0xFF00)>> 8);
    //if (crc16 == *(unsigned short int *)&dataptr[leng-2]) {
    if ( (crc16 & 0xFF) == dataptr[leng-1]  &&  ((crc16 & 0xFF00)>> 8) == dataptr[leng-2]) {
        printf("%02X:%04X ---read check ok ---  \n\n", cmd, crc16 );
        lswrx.len = idx;   // address + ... + CRC16
        lswrx.status = LSW_STATUS_OK;
        return true;
    }else {
        lswrx.status = LSW_STATUS_CMD_FAIL;
        printf("%02X:%04X ---read check failed ---target %04X  \n\n", cmd, crc16, *(unsigned short int *)&dataptr[leng-2] );
        return false;
    }

}


unsigned int lsw_get_read_bytes(lswcmds_e cmd){
	unsigned int  len = 0; 	//expected number of bytes
	  switch ( cmd) {                         //no param needed for such cmd ... group 1
	            case LSW_POLLING_STATUS:  len = 2; 	break;
	            case LSW_GET_DEV_INFO:		len = 59; 	break;// to test print LSW  cmd=31 read 66 bytes 31 39 44 69 43 6F 6E 20 46 69 62 65 72 6F 70 74 69 63 73 20 49 6E 63 2C 56 58 20 31 78 4E 2C 46 57 20 39 37 33 37 38 20 52 65 76 2E 41 31 2C 31 34 41 30 4C 56 32 44 30 30 31 33 3A E2 FF FF FF FF FF
	            case LSW_GET_FW_VER:		len = 5;	break;
	            case LSW_GET_SN:			len = 14;	break;  // to test print LSW  cmd=33 read 18 bytes 33 0C 31 34 41 30 4C 56 32 44 30 30 31 33 5E 09 FF FF
	            case LSW_GET_FW_PART_NO:	len = 9;	break;	// to test print LSW  cmd=35 read 11 bytes 35 07 39 37 33 37 38 41 31 41 38
	            case LSW_GET_HW_PART_NO:	len = 12;	break;	// to test print
	            case LSW_SET_I2C_ADDR:		len = 2;	break;  //LSW  cmd=37 read 2 bytes 37 00
	            case LSW_RESET:				len = 0;	break;
	            case LSW_GET_DEV_DIMENSION:	len = 3;	break;  // 0x70 ok
		  case LSW_SET_IN_OUT:						//0x76 quiet
		  case LSW_GET_OUT_FOR_IN:						//0x77 ok
		  case LSW_SET_OUT_FOR_CH1:   len = 2;	break;     // 0x77  quiet
	            case LSW_GET_OUT_FOR_CH1:		len = 3;                   //0x79 ok 
	  }
	  len += 2;   // plus the CRC16
	  return len;
}

unsigned short int CRC16 (const unsigned char *nData, 
            int wLength     // in bytes
            )    //  from https://www.modbustools.com/modbus_crc16.html, only change types
{
    static const unsigned short int wCRCTable[] = {
        0X0000, 0XC0C1, 0XC181, 0X0140, 0XC301, 0X03C0, 0X0280, 0XC241,
        0XC601, 0X06C0, 0X0780, 0XC741, 0X0500, 0XC5C1, 0XC481, 0X0440,
        0XCC01, 0X0CC0, 0X0D80, 0XCD41, 0X0F00, 0XCFC1, 0XCE81, 0X0E40,
        0X0A00, 0XCAC1, 0XCB81, 0X0B40, 0XC901, 0X09C0, 0X0880, 0XC841,
        0XD801, 0X18C0, 0X1980, 0XD941, 0X1B00, 0XDBC1, 0XDA81, 0X1A40,
        0X1E00, 0XDEC1, 0XDF81, 0X1F40, 0XDD01, 0X1DC0, 0X1C80, 0XDC41,
        0X1400, 0XD4C1, 0XD581, 0X1540, 0XD701, 0X17C0, 0X1680, 0XD641,
        0XD201, 0X12C0, 0X1380, 0XD341, 0X1100, 0XD1C1, 0XD081, 0X1040,
        0XF001, 0X30C0, 0X3180, 0XF141, 0X3300, 0XF3C1, 0XF281, 0X3240,
        0X3600, 0XF6C1, 0XF781, 0X3740, 0XF501, 0X35C0, 0X3480, 0XF441,
        0X3C00, 0XFCC1, 0XFD81, 0X3D40, 0XFF01, 0X3FC0, 0X3E80, 0XFE41,
        0XFA01, 0X3AC0, 0X3B80, 0XFB41, 0X3900, 0XF9C1, 0XF881, 0X3840,
        0X2800, 0XE8C1, 0XE981, 0X2940, 0XEB01, 0X2BC0, 0X2A80, 0XEA41,
        0XEE01, 0X2EC0, 0X2F80, 0XEF41, 0X2D00, 0XEDC1, 0XEC81, 0X2C40,
        0XE401, 0X24C0, 0X2580, 0XE541, 0X2700, 0XE7C1, 0XE681, 0X2640,
        0X2200, 0XE2C1, 0XE381, 0X2340, 0XE101, 0X21C0, 0X2080, 0XE041,
        0XA001, 0X60C0, 0X6180, 0XA141, 0X6300, 0XA3C1, 0XA281, 0X6240,
        0X6600, 0XA6C1, 0XA781, 0X6740, 0XA501, 0X65C0, 0X6480, 0XA441,
        0X6C00, 0XACC1, 0XAD81, 0X6D40, 0XAF01, 0X6FC0, 0X6E80, 0XAE41,
        0XAA01, 0X6AC0, 0X6B80, 0XAB41, 0X6900, 0XA9C1, 0XA881, 0X6840,
        0X7800, 0XB8C1, 0XB981, 0X7940, 0XBB01, 0X7BC0, 0X7A80, 0XBA41,
        0XBE01, 0X7EC0, 0X7F80, 0XBF41, 0X7D00, 0XBDC1, 0XBC81, 0X7C40,
        0XB401, 0X74C0, 0X7580, 0XB541, 0X7700, 0XB7C1, 0XB681, 0X7640,
        0X7200, 0XB2C1, 0XB381, 0X7340, 0XB101, 0X71C0, 0X7080, 0XB041,
        0X5000, 0X90C1, 0X9181, 0X5140, 0X9301, 0X53C0, 0X5280, 0X9241,
        0X9601, 0X56C0, 0X5780, 0X9741, 0X5500, 0X95C1, 0X9481, 0X5440,
        0X9C01, 0X5CC0, 0X5D80, 0X9D41, 0X5F00, 0X9FC1, 0X9E81, 0X5E40,
        0X5A00, 0X9AC1, 0X9B81, 0X5B40, 0X9901, 0X59C0, 0X5880, 0X9841,
        0X8801, 0X48C0, 0X4980, 0X8941, 0X4B00, 0X8BC1, 0X8A81, 0X4A40,
        0X4E00, 0X8EC1, 0X8F81, 0X4F40, 0X8D01, 0X4DC0, 0X4C80, 0X8C41,
        0X4400, 0X84C1, 0X8581, 0X4540, 0X8701, 0X47C0, 0X4680, 0X8641,
        0X8201, 0X42C0, 0X4380, 0X8341, 0X4100, 0X81C1, 0X8081, 0X4040 
    };   //256 word16

    unsigned char nTemp;
    unsigned short int wCRCWord = 0xFFFF;

    while (wLength--)
    {
       nTemp = *nData++ ^ wCRCWord;   // byte array  ^ LSB byte
       wCRCWord >>= 8;
       wCRCWord ^= wCRCTable[nTemp];
    }
    return wCRCWord;
}

/* Testing samples (python generated):
        address 0x73
        get status          30 C4 4B
        get status READ back 4 bytes 30 00 F7 D5
        
        get device info     31 04 8A
        set to output       76 01 00 26 D7
        set to output 1     76 01 01 E6 16
        set to output 2     76 01 02 E7 56
online CRC calculator: https://crccalc.com/
*/

static void help(void)
{
	fprintf(stderr,
		"Usage: i2cset [-f] [-y] [-m MASK] [-r] I2CBUS CHIP-ADDRESS DATA-ADDRESS [VALUE] ... [MODE]\n"
		"  I2CBUS is an integer or an I2C bus name\n"
		"  ADDRESS is an integer (0x03 - 0x77)\n"
		"  MODE is one of:\n"
		"    c (byte, no value)\n"
		"    b (byte data, default)\n"
		"    w (word data)\n"
		"    i (I2C block data)\n"
		"    s (SMBus block data)\n"
		"    Append p for SMBus PEC\n");
	exit(1);
}

/*static const std::string g_bus_name[2] = {
    "/dev/i2c6",  // Port  labelled as "I2C3"  for laser switch
    "/dev/i2c5",  // Port  for laser power
};*/


int lsw_api_run_cmd (lsw_usercmd_e  command, uint8_t param1,  uint8_t param2){
	lsw_tx_prepare(&vcmds[command], param1, param2);
	pthread_mutex_lock(&p_mutex[P_LASER_SWITCH]);
	if (i2c_addr_select(I2C_ADDR_LSW) < 0 ) return -1;
	int bytes = write(fd, &lswtx.data[1],  lswtx.len - 1);
	//int bytes = write(fd, &lswtx.data[0],  lswtx.len - 1, lswtx.len );
	printf("LSW cmd=%02X write %d bytes addr=%02X  content=", lswtx.command, lswtx.len - 1, lswtx.data[0]);
	for(int j = 1; j < lswtx.len; j++) printf (" %02X", lswtx.data[j]); printf("\n");
	pthread_mutex_unlock(&p_mutex[P_LASER_SWITCH]);
	printf(" to tx %d bytes, wrote %d bytes ... done\n", lswtx.len -1 , bytes);
	if ( bytes != (lswtx.len - 1) ) {  perror("write command bytes failed");  return  (-1); }
	//usleep(50000);
	return 0;
}

bool lsw_api_check_cmd_response (lsw_usercmd_e  command){
	unsigned char lswbuf[LSW_CMD_MAX_LENGTH];
	pthread_mutex_lock(&p_mutex[P_LASER_SWITCH]);
	if (i2c_addr_select(I2C_ADDR_LSW) < 0 ) return false;
	int bytes = read(fd, lswbuf, lsw_get_read_bytes(vcmds[command].value));
	pthread_mutex_unlock(&p_mutex[P_LASER_SWITCH]);
	printf("LSW  cmd=%02X read %d bytes", lswbuf[0], bytes ); 
	for(int i = 0; i < bytes; ){printf(" %02X", lswbuf[i++]);	} printf("\n");
	return lsw_rx_check(vcmds[command].value, lswbuf, bytes);
}


void * thread_laserswitch(void * param){  // param can be casted to corresponding struct if needed
	lsw_api_init();
	
	uint8_t p1 = 0, p2 = 0, err = 0; 
	/*  do NOT delete these lines  --- testing purpose
	for (int k = 0; k < UCMD_TOTAL_NUM; k++){
		if ( vcmds[k].value == LSW_SET_I2C_ADDR) p1 = 0x73;
		else if ( vcmds[k].value == LSW_GET_OUT_FOR_IN) p1 = 0x01;
		else if ( vcmds[k].value == LSW_SET_OUT_FOR_CH1) p1 =  (k % 8) +5;  // CH12~CH9 ?	CH0 is parking mode
		else if ( vcmds[k].value == LSW_SET_IN_OUT) { p1 = 0x01;  p2 =	 (k % 8) + 2;}
		else { p1 = 0; p2 = 0; }	//clean up the leftover
		lsw_api_run_cmd(k, p1, p2);
	#if 0
		lsw_tx_prepare(&vcmds[k], p1, p2);
		printf("LSW cmd=%02X write %d bytes %02X  %02X	%02X  %02X \n",
				 lswtx.command, lswtx.len, lswtx.data[0], lswtx.data[1], lswtx.data[2], lswtx.data[3]);
		
		int bytes = write(fd, &lswtx.data[1],  lswtx.len - 1);
		//int bytes = write(fd, &lswtx.data[0],  lswtx.len );
		printf(" write %d bytes ... done\n", bytes);
		usleep(50000);
	
		int bytes = read(fd, lswbuf, lsw_get_read_bytes(vcmds[k].value));
		printf("LSW  cmd=%02X read %d bytes", lswbuf[0], bytes ); for(int k = 0; k < bytes; ){printf(" %02X", lswbuf[k++]); } printf("\n");
		lsw_rx_check(vcmds[k].value, lswbuf, bytes);
	#endif
		usleep(50000); // wait for 50ms  -> 200ms
		lsw_api_check_cmd_response(k);
	}
	p1 = 2;
	for (int j = 0; j < 8; j++){
		lsw_api_run_cmd(UCMD_SET_OUT_FOR_CH1, p1++, 0);
		usleep(50000);
		lsw_api_check_cmd_response(UCMD_SET_OUT_FOR_CH1);
		usleep(50000);
		lsw_api_run_cmd(UCMD_GET_OUT_FOR_CH1, 1, 0);
		usleep(50000);
		lsw_api_check_cmd_response(UCMD_GET_OUT_FOR_CH1);
		
		usleep(1000000);
		p1 %= 9;
		if (p1 == 1) p1 = 2;  // skip 1
	}
	*/
	while(p_running_flags[P_LASER_SWITCH]){
		pthread_mutex_lock(&p_mutex[P_LASER_SWITCH]);
		while(!new_flags[P_LASER_SWITCH]){
			pthread_cond_wait(&p_cond[P_DISPLAY_RX], &p_mutex[P_LASER_SWITCH]);
		}
		printf("LSW received a new command %d p1=%u ...  ^_^\n", rx_obj.type,  (unsigned int)strtoul(rx_obj.value, NULL, 0));
		new_flags[P_LASER_SWITCH] = false;
		pthread_mutex_unlock(&p_mutex[P_LASER_SWITCH]);
		
		if (lsw_api_run_cmd(UCMD_SET_OUT_FOR_CH1,  strtoul(rx_obj.value, NULL, 0), 0)) err = 1;
		usleep(20000);
		if (!lsw_api_check_cmd_response(UCMD_SET_OUT_FOR_CH1)) err =2;
		usleep(20000);
		if (lsw_api_run_cmd(UCMD_GET_OUT_FOR_CH1, 1, 0)) err = 3;
		usleep(20000);
		if (!lsw_api_check_cmd_response(UCMD_GET_OUT_FOR_CH1)) err = 4;

		if (err == 0 ||err == 2){
			new_flags[P_DISPLAY_TX] = true;
			tx_obj.type = rx_obj.type;  
			tx_obj.len = rx_obj.len + 3;  
			int i = 0;
			printf("---> ");
			tx_obj.value[i++] = rx_obj.value[0]; 
			tx_obj.value[i++] = ' ';
			tx_obj.value[i++] = 'O';
			tx_obj.value[i++] = 'k';
			tx_obj.value[i++] = '\0';
			pthread_cond_signal(&p_cond[P_DISPLAY_TX]);
		} else {
			printf("-----no----->>>  %d\n", err);
		}
	}
	lsw_api_stop();
}


void * thread_uart_tx(void * param){  // param can be casted to corresponding struct if needed
		struct UartDevice dev;
		int rc;
		char tx_buf[312];
	
		dev.filename = (char *)"/dev/ttyUSB0";	  // imx6dl-g3-sd, not ttymxc1 this side
		dev.rate = B115200;
	
		if (rc = uart_start(&dev, false) )	{ // first API
			perror("uart_start open for tx and configure failed");
			return NULL;
		}
		printf("UART DEMO transmitter\r\n");		// '\r' added if Windows PC is used in the testing


	
		while (p_running_flags[P_DISPLAY_TX]) {
			pthread_mutex_lock(&p_mutex[P_DISPLAY_TX]);
			while(!new_flags[P_DISPLAY_TX]){
				pthread_cond_wait(&p_cond[P_DISPLAY_TX], &p_mutex[P_DISPLAY_TX]);
			}
			new_flags[P_DISPLAY_TX] = false;
			printf("UART received a new TX request type=%d len=%d v=%s ...  O_O\n", 
							tx_obj.type, tx_obj.len, tx_obj.value);
	
				//tc++;
				snprintf(tx_buf, MAX_LOOPBACK_SIZE, "%05d,%05d,%s\r\n", 
						tx_obj.type, tx_obj.len, &tx_obj.value[0]);  // tx API
				int idx = 0,  txtotal = 5+1+5+1+tx_obj.len + 2;
				while (txtotal > 0) {
					idx += uart_write(&dev, &tx_buf[idx]);
					txtotal -=idx;
				}

			pthread_mutex_unlock(&p_mutex[P_DISPLAY_TX]);
			//sleep(5);
		}
		uart_stop(&dev);  // end API
}

void * thread_uart_rx(void * param){  // param can be casted to corresponding struct if needed
		struct UartDevice dev;
		int rc, cnt = 0;
		dev.filename = (char *)"/dev/ttyUSB0";	  // imx6dl-g3-sd,  display side "ttymxc1"
		dev.rate = B115200;
		if (rc = uart_start(&dev, false) )	{ // first API
			perror("uart_start open for rx and configure failed");
			return NULL;
		}
	
		char read_data[MAX_READ_SIZE];
		size_t read_data_len;
		printf("----------UART  receiver --------\r\n");		// '\r' added if Windows PC is used in the testing
	
		while (p_running_flags[P_DISPLAY_RX]) {
			pthread_mutex_lock(&p_mutex[P_DISPLAY_RX]);
			read_data_len = uart_read(&dev, read_data, MAX_READ_SIZE);
		
			if (read_data_len >= 10 ) {
				printf("#%4d:\trxed %d bytes ... %s\n", ++cnt, (int)read_data_len, read_data);  // %04d,  1 vs. 0001
				sscanf(read_data, "%05d,%05d,",(int*) &(rx_obj.type), (int*)&(rx_obj.len));
				if (rx_obj.len > 0) {
					sscanf(read_data, "%05d,%05d,%s,\r\n", (int*)&rx_obj.type, (int*)&rx_obj.len, &rx_obj.value[0]);
				}
				printf(" rxed type=%d, len=%d, value=%s\n", rx_obj.type, rx_obj.len, rx_obj.value);
			}
			pthread_mutex_unlock(&p_mutex[P_DISPLAY_RX]);

			if(rx_obj.type == UCMD_SET_OUT_FOR_CH1) {
				new_flags[P_LASER_SWITCH] = true;
				pthread_cond_signal(&p_cond[P_DISPLAY_RX]);
			}
		}
		uart_stop(&dev);  // end API
}

int strncmp_insensitive(char * p1, char *p2, int num){
	int ret = 0, idx = 0;
	while ( (idx < num) && (*p1) && (*p2)){
		ret = tolower(*p1) - tolower(*p2);
		if(ret != 0) break;
		else {
			p1++; p2++; idx++;
		}
	}
	if (idx < num && ret == 0) {  // at least one or two finished earlier
		if (*p1 != '\0') { ret = 1; }   // NULL, warning: comparison between pointer and integer
		else if (*p2 != '\0') { ret = -1;}
		else {ret = 0 ; } // no change, both NULL,
		// none is NULL, not existing
	}
	return ret;
}

struct dac_cfg {
	uint8_t buf[16];
	uint8_t n;
	uint16_t data;
	dac_cmd_e cmd;
	dac_ref_e ref;
	dac_pd_e pd;
	dac_gain_e gain;
	bool scriptflag;
	uint8_t bridge;
	uint8_t addr;
	uint8_t beamid;    // output channel of  switch
	//laser seed: ADC[2] DAC[2] TA: ADC[2] DAC[1]  TEC: TMP[2??],  LaserSwitch[1], I2CBridge[1],  DESER[2], SER[8],camera sensors[8],
	uint32_t  selftest_bmp;  //  each bit0 bit1 bit2 ... has its own predefined hw component check, 0xFFFF FFFF to test all, 0 for none
}cobj;

void cmd_line_args_parser(int argc, char * argv[]){
	cobj.scriptflag = false;
	for (int i = 0; i < argc; i++) {
		if(strncmp_insensitive(argv[i], (char*)"-On", 3) == 0) {
			cobj.pd = DAC_CFG_POWERON;
		} else if(strncmp_insensitive(argv[i], (char*)"-Off", 4) == 0 ) {
			cobj.pd = DAC_CFG_PD_1K;
		} else if(strncmp_insensitive(argv[i], (char*)"-Data", 5) == 0) {
			cobj.data = (uint16_t)strtoul(argv[i+1], NULL, 0); i++;
		}else if(strncmp_insensitive(argv[i], (char*)"-Cmd", 4) == 0) {
			cobj.cmd = (dac_cmd_e)strtoul(argv[i+1], NULL, 0); i++;
		}else if(strncmp_insensitive(argv[i], (char*)"-Ref", 4) == 0) {
			cobj.ref = (dac_ref_e)strtoul(argv[i+1], NULL, 0); i++;
		}else if(strncmp_insensitive(argv[i], (char*)"-Gain", 5) == 0) {
			cobj.gain = (dac_gain_e)strtoul(argv[i+1], NULL, 0); i++;
		}else if(strncmp_insensitive(argv[i], (char*)"-ScriptOnly", 7) == 0) {
			cobj.scriptflag = true;
		}else if(strncmp_insensitive(argv[i], (char*)"-Bridge", 5) == 0) {
			cobj.bridge = (uint8_t)strtoul(argv[i+1], NULL, 0);    //0x10 CH4 for laser seed, CH4 0x20 laser TA
			i++;
		}else if(strncmp_insensitive(argv[i], (char*)"-Client", 5) == 0 || strncmp_insensitive(argv[i], (char*)"-Addr", 5) == 0) {
			cobj.addr = (uint8_t)strtoul(argv[i+1], NULL, 0);    //laser seed:0x62 not used, 0x63 seed current, TA: 0x62, 0x63 n/a
			i++;
		}else if(strncmp_insensitive(argv[i], (char*)"-Beam", 5) == 0 || strncmp_insensitive(argv[i], (char*)"-Channel", 7) == 0) {  // -Beam 1~8 or 0
			cobj.beamid = (uint8_t)strtoul(argv[i+1], NULL, 0);	 //laser seed:0x62 not used, 0x63 seed current, TA: 0x62, 0x63 n/a
			i++;
		}else if(strncmp_insensitive(argv[i], (char*)"-Selftest", 8) == 0 || strncmp_insensitive(argv[i], (char*)"-Selfcheck", 9) == 0) {  // -Beam 1~8 or 0
			cobj.selftest_bmp = (uint8_t)strtoul(argv[i+1], NULL, 0);  // 1 to turn on one selftest, 0 bit to turn off one
			i++;
		}else{}
	}

	if (cobj.beamid >= 0 && cobj.beamid <=8){		// simulate or inject an user input on touchdisplay
			rx_obj.type = UCMD_SET_OUT_FOR_CH1;
			rx_obj.len = 1;
			rx_obj.value[0] = cobj.beamid;
			new_flags[P_LASER_SWITCH] = true;
			pthread_cond_signal(&p_cond[P_DISPLAY_RX]);
			usleep(20000);
	}

	// Script example:
	// ./lsw_test -Bridge 0x10 -Client 0x63 -Cmd 4 -On -Ref 0 -Gain 0 -Script --> 6.4 write volatile cfg bits cmd has no data
	// sleep 2
	// ./lsw_test -Bridge 0x10 -Client 0x63 -Cmd 4 -Off -Ref 0 -Gain 0 -Script
	// ./lsw_test -Bridge 0x10 -Client 0x63 -Cmd 3 -Data 320  -Ref 0 -Gain 0 -Script --> change the DAC value, laser current, no output yet
	// ./lsw_test -Bridge 0x10 -Client 0x63 -Cmd 4 -On -Ref 0 -Gain 0 -Script   --> flip ON the DAC output
	if(cobj.scriptflag && argc > 3){
		if (fd < 0) {
			fd = open("/dev/i2c-6",  O_RDWR);  //g_bus_name[busid].c_str()

			if(fd  <  0) {
				perror(" unable to open this i2c bus\n");
				return ;
			}
		}
		printf(" the fd for i2c6 is %d\n", fd);

		if ( ioctl(fd, I2C_SLAVE, I2C_ADDR_BRIDGE) < 0 ){
			perror(" io ctl  i2c BRIDGE slave address failed\n");
			return;
		}else{
			printf("\n io ctl  i2c BRIDGE slave address ok ... %02X\n", I2C_ADDR_BRIDGE);
			uint8_t buf[16], len;
			uint16_t datav, dv2;
			buf[0] = I2C_ADDR_BRIDGE*2;
			buf[1] = cobj.bridge;

			int n = write(fd, &buf[1], 1);
			printf(" wrote %d bytes to BRIDGE\n", n);
			buf[0] = 0;  buf[1] = 0; // clear up
			n = read (fd, buf, 4);
			printf("BRIDGE read %d bytes %02x %02x %02x %02x\n", n, buf[0], buf[1], buf[2], buf[3]);

			if ( ioctl(fd, I2C_SLAVE, cobj.addr) < 0 ){
				perror(" io ctl  i2c  laser-xx slave address failed\n");
				return ;
			}
			printf(" io ctl  i2c laser-xx slave address ok ... %02X\n", cobj.addr);

			mcp4716_dac_cmd_codec(buf, &len, cobj.data, cobj.cmd, cobj.ref, cobj.pd, cobj.gain);
			n = write(fd, &buf[1], len);
			usleep(50000);
			printf(" wrote %d bytes ... executed cmd=%d  pd=%d\n",  n, cobj.cmd, cobj.pd);

			n = read(fd, buf, 14);
			printf("laser DAC-check read\n");
			mcp4716_dac_cmd_codec(buf,  (uint8_t*)&n, 0, DAC_CMD_READ, DAC_CFG_VREF_UNBUFFERED_VDD, DAC_CFG_POWERON, DAC_CFG_GAIN_1X);
			usleep(20000);   //20ms, after DAC WR, immediate RD,  inconsistency between volatile vs EEPROM

		}
		exit(0);
	}
}

static unsigned long tmrcnt = 0;
#define OW_LASER_CHK_STEP		(15)		// in seconds
#define OW_GEN2_LASER_CHK_DURATION    (60*60*24*7/OW_LASER_CHK_STEP)     // 7 days
#define OW_GEN2_LASER_SEED_CH    (0X10)   		// channle 4 as of 2022.01
#define OW_GEN2_LASER_TA_CH  (0X20)

typedef enum chkcomp{
	CHK_LS_CURRENT,
	CHK_LS_BRIGHTNESS,
	CHK_TA_CURRENT,
	CHK_TA_BRIGHTNESS,
	CHK_TEC_1,
	CHK_TEC_2,
	CHK_MAX
}chk_component_e;
#define LOG_CHKBUF_LEN  (60)
int logbuf[CHK_MAX][LOG_CHKBUF_LEN];   // ring-buf

int  laser_i2c_read(int8_t channel, int8_t addr7 ){
	uint8_t buf[16];

	//i2c_switch_channel(channel); //OW_GEN2_LASER_SEED_CH);
	i2c_addr_select(addr7);
	memset(buf, 0, 16);
	int n = read(fd, buf, 12);
	int mv = (((buf[0] << 8) + buf[1]) >> 2)*ADC_REF_VOLT/(1 << ADC_LENGTH);
	printf("periodic seed ADC %02X-read  %d bytes  %02x %02x %02x %02x %02x %02x ... %d mV\n",	addr7,	n,
			  buf[0], buf[1], buf[2], buf[3], buf[4], buf[5], mv);
	return mv;
}
void laser_timeout_checker(int sig){

	if (tmrcnt  > OW_GEN2_LASER_CHK_DURATION){
		//stop the timer
		struct itimerval  itv;
		itv.it_interval.tv_usec = 0;
		itv.it_interval.tv_sec = 0;
		itv.it_value.tv_usec = 0;
		itv.it_value.tv_sec = 0;
		setitimer (ITIMER_REAL, &itv, NULL);
	} else{
		i2c_switch_channel(OW_GEN2_LASER_SEED_CH);
		logbuf[CHK_LS_CURRENT][tmrcnt%LOG_CHKBUF_LEN] = laser_i2c_read(OW_GEN2_LASER_SEED_CH, I2C_ADDR_ADC1);
		logbuf[CHK_LS_BRIGHTNESS][tmrcnt%LOG_CHKBUF_LEN] = laser_i2c_read(OW_GEN2_LASER_SEED_CH, I2C_ADDR_ADC2);

		i2c_switch_channel(OW_GEN2_LASER_TA_CH);
		logbuf[CHK_TA_CURRENT][tmrcnt%LOG_CHKBUF_LEN] = laser_i2c_read(OW_GEN2_LASER_TA_CH, I2C_ADDR_ADC1);
		logbuf[CHK_TA_BRIGHTNESS][tmrcnt%LOG_CHKBUF_LEN] = laser_i2c_read(OW_GEN2_LASER_TA_CH, I2C_ADDR_ADC2);
	}
	tmrcnt++;
}

void * thread_laser_system(void * param){
	signal(SIGALRM, laser_timeout_checker);
	//http://www.gnu.org/software/libc/manual/html_node/Setting-an-Alarm.html
	struct itimerval old, itv;
	itv.it_interval.tv_usec = 0;
	itv.it_interval.tv_sec = OW_LASER_CHK_STEP;		//monitoring the TEC value every 3 seconds? TODO  laser people parameter
	itv.it_value.tv_usec = 0;
	itv.it_value.tv_sec = (long int) 1;
	setitimer (ITIMER_REAL, &itv, &old);
	while (p_running_flags[P_LASER_SYSTEM]) {
	}
}

void * thread_power_distribution(void * param){
	while (p_running_flags[P_POWER_SRC]) {
	}
}


int main(int argc, char *argv[])
{
	cobj.pd = DAC_CFG_PD_1K;
	cobj.ref = DAC_CFG_VREF_UNBUFFERED_VDD;
	cobj.gain = DAC_CFG_GAIN_1X;
	cobj.cmd = DAC_CMD_READ;
	cobj.scriptflag = false;
	cobj.beamid = 0xFF;
	cobj.selftest_bmp = 0xFFFFFFFF;   // default to test all components, cmd line options can turn them off individually

        //cmd_line_args_parser(argc, argv);

	pthread_mutex_init(&p_mutex[P_LASER_SWITCH], NULL);
	p_running_flags[P_LASER_SWITCH] = true;
	// not used: the 2nd,  pthread_attr_t, pthread_attr_init, pthread_attr_setstacksize
	pthread_create(&p_ids[P_LASER_SWITCH], NULL, thread_laserswitch, NULL);

	pthread_mutex_init(&p_mutex[P_DISPLAY_TX], NULL);
	pthread_cond_init(&p_cond[P_DISPLAY_TX], NULL);
	p_running_flags[P_DISPLAY_TX] = true;
	pthread_create(&p_ids[P_DISPLAY_TX], NULL, thread_uart_tx, NULL);

	pthread_mutex_init(&p_mutex[P_DISPLAY_RX], NULL);
	pthread_cond_init(&p_cond[P_DISPLAY_RX], NULL);
	p_running_flags[P_DISPLAY_RX] = true;
	pthread_create(&p_ids[P_DISPLAY_RX], NULL, thread_uart_rx, NULL);

	cmd_line_args_parser(argc, argv);

	pthread_mutex_init(&p_mutex[P_LASER_SYSTEM], NULL);
	pthread_cond_init(&p_cond[P_LASER_SYSTEM], NULL);
	p_running_flags[P_LASER_SYSTEM] = true;
	pthread_create(&p_ids[P_LASER_SYSTEM], NULL, thread_laser_system, NULL);

	pthread_mutex_init(&p_mutex[P_POWER_SRC], NULL);
	pthread_cond_init(&p_cond[P_POWER_SRC], NULL);
	p_running_flags[P_POWER_SRC] = true;
	pthread_create(&p_ids[P_POWER_SRC], NULL, thread_power_distribution, NULL);

	for(int i = 0; i < P_THREADS_TOTAL; i++){
		if ( p_ids[i] > 0 ){
			pthread_join(p_ids[i], NULL);
		}
	}

	pthread_mutex_destroy(&p_mutex[P_LASER_SWITCH]);
	return 0;
}
