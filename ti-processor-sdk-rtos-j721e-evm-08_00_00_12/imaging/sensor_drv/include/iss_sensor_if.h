/*
 *
 * Copyright (c) 2020 Texas Instruments Incorporated
 *
 * All rights reserved not granted herein.
 *
 * Limited License.
 *
 * Texas Instruments Incorporated grants a world-wide, royalty-free, non-exclusive
 * license under copyrights and patents it now or hereafter owns or controls to make,
 * have made, use, import, offer to sell and sell ("Utilize") this software subject to the
 * terms herein.  With respect to the foregoing patent license, such license is granted
 * solely to the extent that any such patent is necessary to Utilize the software alone.
 * The patent license shall not apply to any combinations which include this software,
 * other than combinations with devices manufactured by or for TI ("TI Devices").
 * No hardware patent is licensed hereunder.
 *
 * Redistributions must preserve existing copyright notices and reproduce this license
 * (including the above copyright notice and the disclaimer and (if applicable) source
 * code license limitations below) in the documentation and/or other materials provided
 * with the distribution
 *
 * Redistribution and use in binary form, without modification, are permitted provided
 * that the following conditions are met:
 *
 * *       No reverse engineering, decompilation, or disassembly of this software is
 * permitted with respect to any software provided in binary form.
 *
 * *       any redistribution and use are licensed by TI for use only with TI Devices.
 *
 * *       Nothing shall obligate TI to provide you with source code for the software
 * licensed and provided to you in object code.
 *
 * If software source code is provided to you, modification and redistribution of the
 * source code are permitted provided that the following conditions are met:
 *
 * *       any redistribution and use of the source code, including any resulting derivative
 * works, are licensed by TI for use only with TI Devices.
 *
 * *       any redistribution and use of any object code compiled from the source code
 * and any resulting derivative works, are licensed by TI for use only with TI Devices.
 *
 * Neither the name of Texas Instruments Incorporated nor the names of its suppliers
 *
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * DISCLAIMER.
 *
 * THIS SOFTWARE IS PROVIDED BY TI AND TI'S LICENSORS "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL TI AND TI'S LICENSORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
#ifndef ISS_SENSORS_IF_H_
#define ISS_SENSORS_IF_H_

/*TODO: This should be set from a build parameter"*/
#define BUILD_TDA4_RVP
//#define BUILD_TDA4_GMSL

#ifdef BUILD_TDA4_RVP



#define SER_0_I2C_ALIAS       (0x64U)
#define SER_1_I2C_ALIAS       (0x66U)
#define SER_2_I2C_ALIAS       (0x68U)
#define SER_3_I2C_ALIAS       (0x6AU)

#define SER_4_I2C_ALIAS       (0x6CU)
#define SER_5_I2C_ALIAS       (0x6EU)
#define SER_6_I2C_ALIAS       (0x70U)
#define SER_7_I2C_ALIAS       (0x72U)

#define SENSOR_0_I2C_ALIAS    (0x40U)
#define SENSOR_1_I2C_ALIAS    (0x42U)
#define SENSOR_2_I2C_ALIAS    (0x44U)
#define SENSOR_3_I2C_ALIAS    (0x46U)

#define SENSOR_4_I2C_ALIAS    (0x47U)
#define SENSOR_5_I2C_ALIAS    (0x4AU)
#define SENSOR_6_I2C_ALIAS    (0x4CU)
#define SENSOR_7_I2C_ALIAS    (0x4EU)

#ifdef BUILD_TDA4_GMSL

#undef SER_0_I2C_ALIAS
#define SER_0_I2C_ALIAS         0x42

#undef SER_1_I2C_ALIAS
#define SER_1_I2C_ALIAS         0x62

#undef SENSOR_0_I2C_ALIAS
#define SENSOR_0_I2C_ALIAS      0x20

#undef SENSOR_1_I2C_ALIAS
#define SENSOR_1_I2C_ALIAS      0x10

#endif /* BUILD_TDA4_GMSL */

#else

#define SENSOR_0_I2C_ALIAS    (0x40U)
#define SENSOR_1_I2C_ALIAS    (0x42U)
#define SENSOR_2_I2C_ALIAS    (0x44U)
#define SENSOR_3_I2C_ALIAS    (0x46U)

#define SENSOR_4_I2C_ALIAS    (0x48U)
#define SENSOR_5_I2C_ALIAS    (0x4AU)
#define SENSOR_6_I2C_ALIAS    (0x4CU)
#define SENSOR_7_I2C_ALIAS    (0x4EU)


#define SER_0_I2C_ALIAS       (0x74U)
#define SER_1_I2C_ALIAS       (0x76U)
#define SER_2_I2C_ALIAS       (0x78U)
#define SER_3_I2C_ALIAS       (0x7AU)

#define SER_4_I2C_ALIAS       (0x6CU)
#define SER_5_I2C_ALIAS       (0x6EU)
#define SER_6_I2C_ALIAS       (0x70U)
#define SER_7_I2C_ALIAS       (0x72U)

#endif /* End of ISS_SENSORS_IF_H_*/

#endif /* End of ISS_SENSORS_IF_H_*/

