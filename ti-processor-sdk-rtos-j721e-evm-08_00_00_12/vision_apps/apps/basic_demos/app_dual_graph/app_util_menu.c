#include "app_util_menu.h"
#include <utils/iss/include/app_iss.h>

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <string.h>

static char util_menu[] = {
    "\n"
    "\n"
    "\n  Openwater Utility - (c) Openwater 2023");
    "\n ========================================================\n");
    "\n"
    "\n u: Utility Menu"
    "\n"
    "\n x: Exit"
    "\n"
}

static char utils_menu0[] = {
    "\n"
    "\n =================================="
    "\n I2C Utilities Menu"
    "\n =================================="
    "\n"
    "\n i 1: I2C Read"
    "\n o 2: I2C Write"
    "\n   3: Set Mode"
    "\n   4: Set Device Address"
    "\n r 5: Set Register Address"
    "\n d 6: Set Data"
    "\n"
    "\n x: Exit"
    "\n"
};

static char utils_menu1[] = {
    "\n"
    "\n 0: 8A8D"
    "\n 1: 8A16D"
    "\n 2: 16A8D"
    "\n 3: 16A16D"
    "\n"
    "\n Enter Choice: "
    "\n"
};

static uint32_t __readLine(uint32_t maxlen, char *buf) {
    int i = 0;
    char ch = 0;

    if(buf == NULL || maxlen <= 0) {
        return 0;
    }

    buf[0] = '\0'; // start with a null string

    for(i = 0; i < maxlen; i++) {
        ch = getchar();
        buf[i] = ch;
        if(buf[i] == '\n' || buf[i] == '\r') {
            buf[i] = '\0';
            break;
        }
    }

    return strlen(buf);
}

void run_d3_utils_menu()
{
    D3UtilsI2C_Command *cmd = malloc(sizeof(D3UtilsI2C_Command));
    cmd->type = D3_I2C_READ;
    cmd->regAddr = 0;
    cmd->regData = 0;
    cmd->devAddr = 0;
    uint8_t done = 0;
    char ch;
    char inputBuffer[8];

    while(done == 0) {
        printf(utils_menu0);
        printf(
            " MODE:%s Device: ADDR:0x%02X, REG:0x%04X, Data:0x%04X\n",
            modeToStr(cmd->type),
            cmd->devAddr,
            cmd->regAddr,
            cmd->regData
        );
        printf(" Enter Choice: \n");
        do {
            ch = getchar();
        } while(ch == '\n' || ch == '\r');
        getchar(); // Eat the newline.
        switch(ch) {
            case 'i':
            case '1':
                printf("READ.\n");
                cmd->direction = D3_I2C_READ;
                appD3I2CMenu(cmd);
                break;
            case 'o':
            case '2':
                printf("WRITE.\n");
                cmd->direction = D3_I2C_WRITE;
                appD3I2CMenu(cmd);
                break;
            case '3':
                printf(utils_menu1);
                ch = getchar();
                switch(ch) {
                    case '0':
                        cmd->type = TRANSACTION_TYPE_8A8D;
                        break;
                    case '1':
                        cmd->type = TRANSACTION_TYPE_8A16D;
                        break;
                    case '2':
                        cmd->type = TRANSACTION_TYPE_16A8D;
                        break;
                    case '3':
                        cmd->type = TRANSACTION_TYPE_16A16D;
                        break;
                    default:
                        printf("Invalid type selected.\n");
                        break;
                }
                break;
            case '4':
                printf("Enter Device Address (7b Hex) and press Enter: ");
                __readLine(8, inputBuffer);
                cmd->devAddr = ((uint8_t) strtol(inputBuffer, NULL, 16)) & 0x7FU;
                break;
            case '5':
            case 'r':
                printf("Enter Register Address (Hex) and press Enter: ");
                __readLine(8, inputBuffer);
                cmd->regAddr = ((uint16_t) strtol(inputBuffer, NULL, 16));
                switch(cmd->type) {
                    case TRANSACTION_TYPE_8A16D:
                    case TRANSACTION_TYPE_8A8D:
                        cmd->regAddr &= 0xFFU;
                        break;
                    default:
                        break;
                }
                break;
            case '6':
            case 'd':
                printf("Enter Data (Hex) and press Enter: ");
                __readLine(8, inputBuffer);
                cmd->regData = ((uint16_t) strtol(inputBuffer, NULL, 16));
                switch(cmd->type) {
                    case TRANSACTION_TYPE_8A16D:
                    case TRANSACTION_TYPE_8A8D:
                        cmd->regData &= 0xFFU;
                        break;
                    default:
                        break;
                }
                break;
            case 'x':
                done = 1;
                break;
            default:
                printf("Unrecognized command.\n");
                break;
        }
    }
}