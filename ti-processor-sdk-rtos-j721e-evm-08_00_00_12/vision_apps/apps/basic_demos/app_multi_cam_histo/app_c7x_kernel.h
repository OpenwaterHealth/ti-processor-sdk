/**************************************************************************
*  Copyright 2021   Neither this file nor any part of it may be used for 
*     any purpose without the written consent of D3 Engineering, LLC.
*     All rights reserved.
*				
*				D 3   E N G I N E E R I N G
*
* File Name   :	app_c7x_kernel.h
* Release     :	1.0
* Author      :	D3 Engineering Technical Staff
* Created     :	09/21/2021
* Revision    :	1.0
* Description :	Header file for the c7x kernel that implements histogram function
* 
*		
***************************************************************************
* Revision History
* Ver	Date			Author		Description 
* 1.0	09/22/2021		VC		Initial version	
***************************************************************************/

#ifndef APP_C7X_KERNEL_H
#define APP_C7X_KERNEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <VX/vx.h>


#define APP_C7X_HISTO_IN_CONFIG_IDX   (0u)

/** \brief Index of input image 1 in parameter list */
#define APP_C7X_HISTO_IN_RAW_IMG_IDX   (1u)

/** \brief Index of output distribution in parameter list */
#define APP_C7X_HISTO_OUT_DISTRIBUTION_IDX          (2u)

/** \brief Index of output mena value in parameter list */
#define APP_C7X_HISTO_OUT_MEAN_IDX          (3u)

/** \brief Index of output standard deviation in parameter list */
#define APP_C7X_HISTO_OUT_SD_IDX          (4u)

/** \brief Total number of parameters for this function */
#define APP_C7X_HISTO_MAX_PARAMS     (5u)

#define TIVX_C7X_HISTO_CMD_ENABLE_CHAN 0
#define TIVX_C7X_HISTO_CMD_CHANGE_PAIR 1
#define TIVX_C7X_HISTO_TIMESTAMP_SYNC 2

/** \brief Image Add kernel name */
#define APP_C7X_KERNEL_HISTO_NAME     "app_c7x_kernel.histo"

#define APP_C7X_HISTO_MAX_NUM_CHANNELS 8
#define APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE 2
#define APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE_MASK ((1<<APP_C7X_HISTO_MAX_NUM_CHANNELS_ENABLED_AT_ONCE)-1)

typedef struct {

    uint32_t enableChanBitFlag;

} tivxHistoParams;

typedef struct {

    uint32_t enableChanBitFlag;

} tivxHistoCmd;

typedef struct {

    uint64_t refTimestamp;

} tivxHistoTimeSync;

/* target kernels register/unregister */
void app_c7x_target_kernel_histo_register(void);
vx_status app_c7x_target_kernel_histo_unregister(void);

/* host side kernel wrapper register/unregister */
vx_status app_c7x_kernel_histo_register(vx_context);
vx_status app_c7x_kernel_histo_unregister(vx_context);

/* kernels node */
vx_node app_c7x_kernel_histo_kernel_node(vx_graph graph, vx_user_data_object config, tivx_raw_image raw_img, vx_distribution img_out, vx_scalar mean, vx_scalar sd);

#ifdef __cplusplus
}
#endif

#endif
