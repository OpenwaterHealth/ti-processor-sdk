#include "memory_map_defines.inc"

#define __VECS                  MCU_R5F_TCMB_VECS
#define __BOOT                  MCU_R5F_TCMB
#define __CORE_IPC_DATA         MCU2_1_IPC_DATA
#define __CORE_EXT_DATA_BASE    MCU2_1_EXT_DATA_BASE
#define __CORE_EXT_DATA         MCU2_1_EXT_DATA
#define __CORE_R5F_MEM_TEXT     MCU2_1_MEM_TEXT
#define __CORE_R5F_MEM_DATA     MCU2_1_MEM_DATA
#define __CORE_DDR_SPACE        MCU2_1_DDR_SPACE

#include "linker_r5f_freertos_common.inc"