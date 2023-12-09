#ifndef D3_UTILS_H_
#define D3_UTILS_H_


typedef enum
{
    TRANSACTION_TYPE_8A8D,
    TRANSACTION_TYPE_8A16D,
    TRANSACTION_TYPE_16A8D,
    TRANSACTION_TYPE_16A16D,
    D3_UTILS_I2C_TRANSACTION_TYPE_FORCE32BITS = 0x7FFFFFFF
} D3_UTILS_I2C_TRANSACTION_TYPE;

typedef enum
{
    D3_I2C_WRITE,
    D3_I2C_READ
} D3_UTILS_I2C_DIRECTION;

typedef struct
{
    D3_UTILS_I2C_DIRECTION direction;
    D3_UTILS_I2C_TRANSACTION_TYPE type;
    uint32_t devAddr;
    uint16_t regAddr;
    uint16_t regData;
} D3UtilsI2C_Command;

void D3Utils_I2CCommandExecute(D3UtilsI2C_Command *cmd);


#endif
