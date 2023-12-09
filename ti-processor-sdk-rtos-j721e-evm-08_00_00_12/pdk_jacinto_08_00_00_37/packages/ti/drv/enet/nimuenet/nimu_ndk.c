/*
 *  Copyright (c) Texas Instruments Incorporated 2020
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *    Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 *    Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the
 *    distribution.
 *
 *    Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * \file  nimu_ndk.c
 *
 * \brief Interface module to support NIMU packet architecture for NDK
 */

/* ========================================================================== */
/*                             Include Files                                  */
/* ========================================================================== */

#include <stdint.h>
#include <stdarg.h>

/* xdc header - should be first file included as due to order dependency */
#include <xdc/std.h>
#include <xdc/runtime/System.h>

/* NDK Dependencies */
#include <ti/ndk/inc/stkmain.h>

#include <ti/drv/enet/include/core/enet_dma.h>

/* This module's header */
#include "ndk2enet.h"
#include "nimu_ndk.h"

/* ========================================================================== */
/*                          Extern Declarations                               */
/* ========================================================================== */

extern PBMQ PBMQ_free;

/* ========================================================================== */
/*                           Macros & Typedefs                                */
/* ========================================================================== */

#define VLAN_TAG_SIZE         (4U)
#define ETH_FRAME_SIZE        (ETH_MAX_PAYLOAD + VLAN_TAG_SIZE)

/* ========================================================================== */
/*                         Structure Declarations                             */
/* ========================================================================== */

/* None */

/* ========================================================================== */
/*                          Function Declarations                             */
/* ========================================================================== */

static int  NIMU_NDK_start(NETIF_DEVICE *pNETIFDevice);

static int  NIMU_NDK_stop(NETIF_DEVICE *pNETIFDevice);

static void NIMU_NDK_poll(NETIF_DEVICE *pNETIFDevice,
                          unsigned int timer_tick);

static int  NIMU_NDK_send(NETIF_DEVICE *pNETIFDevice,
                          PBM_Handle hPkt);

static void NIMU_NDK_pktService(NETIF_DEVICE *pNETIFDevice);

static int  NIMU_NDK_ioctl(NETIF_DEVICE *pNETIFDevice,
                           unsigned int cmd,
                           void *pBuf,
                           unsigned int size);

/* ========================================================================== */
/*                            Global Variables                                */
/* ========================================================================== */

NETIF_DEVICE gNetIfDeviceObj;

/* ========================================================================== */
/*                          Function Definitions                              */
/* ========================================================================== */

/*!
 *  @b NIMU_NDK_Init
 *  @n
 *  The function is used to initialize and register ENET
 *  with the Network Interface Management Unit (NIMU)
 *
 *  \param[in]  hEvent
 *      Stack Event Handle.
 *
 *  \retval
 *      Success -   0
 *  \retval
 *      Error   -   <0
 */
int NIMU_NDK_init(STKEVENT_Handle hEvent)
{
    NETIF_DEVICE *pNETIFDevice;

    pNETIFDevice = &gNetIfDeviceObj;

    /* Initialize the allocated memory block. */
    memset(pNETIFDevice, 0, sizeof(NETIF_DEVICE));

    /*
     * Tell stack NOT to call mmFree for us on this instance!
     * This allows driver to have full control over the means of
     * allocating/freeing for this NIMU device instance.
     */
    pNETIFDevice->flags = NIMU_DEVICE_NO_FREE;

#ifdef NDK_CHECKSUM_SUPPORT
    {
        /* Tell NDK, HW checksum offload is enabled for both RX and TX */

        /* Below flag is mainly used for TX. When set in combination with other TX flags,
         * the NDK stack will update the PBM checksum fields (csStartPos, csNumBytes, csInsertPos),
         * for each PBM packet, which are needed for the h/w’s TX CS calculations.*/
        pNETIFDevice->flags |= NIMU_DEVICE_HW_CHKSM_PARTIAL;

        /* For TX the J721E hardware supports checksum computation based on byte offset and
         * length in descriptor, effectively enabling support for all protocols.
         * Now as hardware supports only one checksum in the full packet. For example TCP header, TCP
         * payload or IP header, we offload TCP and UDP payload as these are most expensive.
         * NDK does other checksums in software */
        pNETIFDevice->flags |= NIMU_DEVICE_ENABLE_HW_CHKSM_TX_UDP|
                               NIMU_DEVICE_ENABLE_HW_CHKSM_TX_TCP;

        /* For RX the J721E hardware supports UDP and TCP checksum offload */
        pNETIFDevice->flags |= NIMU_DEVICE_ENABLE_HW_CHKSM_RX_UDP|
                               NIMU_DEVICE_ENABLE_HW_CHKSM_RX_TCP;
    }
#endif

    /* Populate the Network Interface Object. */
    strcpy(pNETIFDevice->name, "eth0");

    /*
     * MTU is i total size of the (IP) packet that can fit into an Ethernet.
     * For Ethernet it is 1500bytes
     */
#if defined(ENABLE_JUMBO_FRAMES)

    /*
     * For TDA hardware maximum packet length for ENET DMA can't be more that 2048
     * due to packet length being 11 bits in descriptors. Hence Jumbo packet of
     * max. 2K can be used
     */
    pNETIFDevice->mtu = 2000U;
#else
    pNETIFDevice->mtu = ETH_FRAME_SIZE - ETHHDR_SIZE - VLAN_TAG_SIZE;
#endif

    /* Stash the STKEVENT_Handle until we use it in the start function */
    pNETIFDevice->pvt_data = (void *)hEvent;

    /* Populate the Driver Interface Functions. */
    pNETIFDevice->start       = &NIMU_NDK_start;
    pNETIFDevice->stop        = &NIMU_NDK_stop;
    pNETIFDevice->poll        = &NIMU_NDK_poll;
    pNETIFDevice->send        = &NIMU_NDK_send;
    pNETIFDevice->pkt_service = &NIMU_NDK_pktService;
    pNETIFDevice->ioctl       = &NIMU_NDK_ioctl;
    /* NIMUAddEthernetHeader is defined in NDK nimu interface file, we reuse that
     * instead of defining our own. Though NIMUs are free to use their own implemntation,
     * using NDK implementation is beneficial as changes in interface are abstracted */
    pNETIFDevice->add_header  = &NIMUAddEthernetHeader;

    /* Register the device with NIMU */
    if (NIMURegister(pNETIFDevice) < 0)
    {
        System_printf("[NIMU_NDK] Error: Unable to register ENET\n");
        return -1;
    }

    System_printf("[NIMU_NDK] Registration of the ENET Successful\n");

    return 0;
}

/*!
 *  @b NIMU_NDK_Start
 *  @n
 *  The function is used to initialize and start ENET
 *  controller and device.
 *
 *  \param[in]  pNETIFDevice
 *      NETIF_DEVICE structure pointer.
 *
 *  \retval
 *      Success -   0
 *  \retval
 *      Error   -   <0
 */
static int NIMU_NDK_start(NETIF_DEVICE *pNETIFDevice)
{
    int retVal = -1;
    Ndk2Enet_Handle hNdk2Enet;

    /* Open the translation layer, which itself opens the hardware driver */
    hNdk2Enet = Ndk2Enet_open((STKEVENT_Handle)pNETIFDevice->pvt_data);

    if (NULL != hNdk2Enet)
    {
        /* Save off a pointer to the translation layer */
        pNETIFDevice->pvt_data = (void *)hNdk2Enet;

        /* Copy the MAC Address into the network interface object here. */
        mmCopy(&pNETIFDevice->mac_address[0], &hNdk2Enet->appInfo.rxInfo.macAddr[0], (uint32_t)6U);

        /* Set the 'initial' Receive ndkRxFilter */
        hNdk2Enet->ndkRxFilter = ETH_PKTFLT_ALL;
        Ndk2Enet_setRx(hNdk2Enet);

        /* Inform the world that we are operational. */
        hNdk2Enet->print("[NIMU_NDK] ENET has been started successfully\n");

        retVal = 0;
    }
    else
    {
        /* Note - Use System_printf here as we are not sure if Ndk2Enet print
         * is set and not null. */
        System_printf("[NIMU_NDK] Failed to start ENET \n");
    }

    return retVal;
}

/*!
 *  @b NIMU_NDK_Stop
 *  @n
 *  The function is used to de-initialize and stop ENET
 *  controller and device.
 *
 *  \param[in]  pNETIFDevice
 *      NETIF_DEVICE structure pointer.
 *
 *  \retval
 *      Success -   0
 *  \retval
 *      Error   -   <0
 */

/*
 * MISRA-C:2004 Rule 16.7 deviation to match the required function signature
 * defined in nimuif.h
 */
static int NIMU_NDK_stop(NETIF_DEVICE *pNETIFDevice)
{
    Ndk2Enet_Handle hNdk2Enet;

    /* Get the pointer to the private data */
    hNdk2Enet = (Ndk2Enet_Handle)pNETIFDevice->pvt_data;

    /* Call low-level close function */
    Ndk2Enet_close(hNdk2Enet);

    /* Clear the NETIF_DEVICE memory block. */
    memset(pNETIFDevice, 0, sizeof(NETIF_DEVICE));

    /* ENET Controller has been stopped. */
    return 0;
}

/*!
 *  @b NIMU_NDK_Poll
 *  @n
 *  The function is used to poll the ENET controller to check
 *  if there has been any activity
 *
 *  \param[in]  pNETIFDevice
 *      NETIF_DEVICE structure pointer.
 *  \param[in]  timer_tick
 *      Flag to poll the driver.
 *
 *  \retval
 *      void
 */
static void NIMU_NDK_poll(NETIF_DEVICE *pNETIFDevice,
                          unsigned int timer_tick)
{
    /* Call the driver's periodic polling function */
    if (timer_tick)
    {
        /* Get the pointer to the private data */
        Ndk2Enet_Handle hNdk2Enet = (Ndk2Enet_Handle)pNETIFDevice->pvt_data;

        uint32_t currLinkedIf, prevLinkedInterface;

        prevLinkedInterface = hNdk2Enet->currLinkedIf;
        currLinkedIf        = hNdk2Enet->currLinkedIf;
        /* Periodic Function to update Link status */
        Ndk2Enet_periodicFxn(hNdk2Enet);

        if (currLinkedIf != prevLinkedInterface)
        {
            /* The linked interface has changed, so update MAC address in the NDK stack */
            mmCopy(pNETIFDevice->mac_address, &hNdk2Enet->appInfo.rxInfo.macAddr, (uint32_t)6U);
        }
    }
}

/*!
 *  @b NIMU_NDK_Send
 *  @n
 *  The function is the interface routine invoked by the NDK stack to
 *  pass packets to the driver.
 *
 *  \param[in]  pNETIFDevice
 *      NETIF_DEVICE structure pointer.
 *  \param[in]  hPkt
 *      Packet to be sent out on wire.
 *
 *  \retval
 *      Success -   0
 *  \retval
 *      Error   -   <0
 */

/*
 * MISRA-C:2004 Rule 16.7 deviation to match the required function signature
 * defined in nimuif.h
 */
static int NIMU_NDK_send(NETIF_DEVICE *pNETIFDevice,
                         PBM_Handle hPkt)
{
    Ndk2Enet_Handle hNdk2Enet;

    /* Get the pointer to the private data */
    hNdk2Enet = (Ndk2Enet_Handle)pNETIFDevice->pvt_data;
    /* If link is not up, simply return */
    if (hNdk2Enet->linkIsUp)
    {
        /* Enqueue the packet */
        PBMQ_enq(&hNdk2Enet->txReadyPBMPktQ, hPkt);
        Ndk2EnetStas_addOne(&gNdk2EnetStats.txReadyPBMPktEnq);

        /* Pass the packet to the translation layer */
        Ndk2Enet_sendTxPackets(hNdk2Enet);
    }

    /* Packet has been successfully transmitted or enqueued to be sent when link comes up */
    return 0;
}

/*!
 *  @b NIMU_NDK_PktService
 *  @n
 *  The function is called by the NDK core stack to receive any packets
 *  from the driver. It is called in the context of the main NDK service
 *  task.
 *
 *  \param[in]  pNETIFDevice
 *      NETIF_DEVICE structure pointer.
 *
 *  \retval
 *      void
 */
static void NIMU_NDK_pktService(NETIF_DEVICE *pNETIFDevice)
{
    Ndk2Enet_Handle hNdk2Enet;
    PBM_Handle hPBMPacket;

    /* Get the pointer to the private data */
    hNdk2Enet = (Ndk2Enet_Handle)pNETIFDevice->pvt_data;

    /* Give all queued packets to the NDK stack */
    hPBMPacket = PBMQ_deq(&hNdk2Enet->rxReadyPBMPktQ);
    while (hPBMPacket != NULL)
    {
        Ndk2EnetStas_addOne(&gNdk2EnetStats.rxReadyPBMPktDq);

        /* Prepare the packet so that it can be passed up the networking stack.
         * If this 'step' is not done the fields in the packet are not correct
         * and the packet will eventually be dropped.  */
        PBM_setIFRx(hPBMPacket, pNETIFDevice);

        /* Pass the packet to the NDK Core stack. */
        NIMUReceivePacket(hPBMPacket);
        hPBMPacket = PBMQ_deq(&hNdk2Enet->rxReadyPBMPktQ);
    }
}

/*!
 *  @b NIMU_NDK_Ioctl
 *  @n
 *  The function is called by the NDK core stack to configure the
 *  driver
 *
 *  \param[in]  pNETIFDevice
 *      NETIF_DEVICE structure pointer.
 *  \param[in]  cmd
 *      Ioctl command.
 *  \param[in]  pBuf
 *      Mac address to be added or deleted.
 *  \param[in]  size
 *      Size of Mac Address.
 *
 *  \retval
 *      Success -   0
 *  \retval
 *      Error   -   <0
 */

/*
 * MISRA-C:2004 Rule 16.7 deviation to match the required function signature
 * defined in nimuif.h
 */
static int NIMU_NDK_ioctl(NETIF_DEVICE *pNETIFDevice,
                          unsigned int cmd,
                          void *pBuf,
                          unsigned int size)
{
    Ndk2Enet_Handle hNdk2Enet;
    uint32_t count = 0;
    uint32_t index;
    uint8_t *mac_address_add_del = (uint8_t *)pBuf;

    /* Get the pointer to the private data */
    hNdk2Enet = (Ndk2Enet_Handle)pNETIFDevice->pvt_data;

    /* Process the command. */
    switch (cmd)
    {
        case (unsigned int)NIMU_ADD_MULTICAST_ADDRESS:
        {
            /* Validate the arguments. */
            if ((pBuf == NULL) || (size != 6U))
            {
                return -NDK_EINVAL;
            }

            /* We need to add an address to the NIMU Network Interface Object's multicast
             * list. Check for duplicate addresses. */
            while (count < hNdk2Enet->MCastCnt)
            {
                uint8_t *mac_address_list = (uint8_t *)&hNdk2Enet->bMCast[count * 6U];

                /* Check for a matching MAC Addresses in the list */
                for (index = 0; index < 6U; index++)
                {
                    if (mac_address_add_del[index] != mac_address_list[index])
                    {
                        break;
                    }
                }

                /* Check if there is a hit or not? */
                if (index == 6U)
                {
                    /* Duplicate MAC address; the address was already present in the list.
                     * This is not an error we will still return SUCCESS here */
                    return 0;
                }

                /* No HIT! Goto the next entry in the device multicast list. */
                count++;
            }

            /* Control comes here implies that the MAC Address needs to be added to the
             * device list. The variable 'count' is pointing to the free location available
             * in which the multicast address can be added. But before we do so check if
             * we dont exceed the upper limit? */
            if (count >= PKT_MAX_MCAST)
            {
                return -NDK_ENOMEM;
            }

            /* Add the multicast address to the end of the list. */
            memcpy(&hNdk2Enet->bMCast[count * 6U], mac_address_add_del, (size_t)6U);
            hNdk2Enet->MCastCnt++;

            /* Configure the Ethernet controller with the new multicast list. */
            Ndk2Enet_setRx(hNdk2Enet);
            break;
        }

        case (unsigned int)NIMU_DEL_MULTICAST_ADDRESS:
        {
            /* Validate the arguments. */
            if ((pBuf == NULL) || (size != 6U))
            {
                return -NDK_EINVAL;
            }

            /* We need to delete an address from the NIMU Network Interface Object's multicast
             * list. First cycle through and make sure the entry exists. */
            while (count < hNdk2Enet->MCastCnt)
            {
                uint8_t *mac_address_list = (uint8_t *)&hNdk2Enet->bMCast[count * 6U];

                /* Match the MAC Addresses  */
                for (index = 0; index < 6U; index++)
                {
                    if (*(mac_address_add_del + index) != *(mac_address_list + index))
                    {
                        break;
                    }
                }

                /* Check if there is a hit or not? */
                if (index == 6U)
                {
                    /* Found the matching entry. We can now delete this! */
                    break;
                }

                /* No HIT! Goto the next entry in the device multicast list. */
                count++;
            }

            /* Did we find a match or not? If not then report the error back */
            if (count == hNdk2Enet->MCastCnt)
            {
                return -NDK_EINVAL;
            }

            /* At this time 'count' points to the entry being deleted. We now need to copy all
             * the entries after the 'del' entry back one space in the multicast array. */
            for (index = count; index < (hNdk2Enet->MCastCnt - (uint32_t)1U); index++)
            {
                memcpy(&hNdk2Enet->bMCast[index * 6U], &hNdk2Enet->bMCast[(index + 1U) * 6U], (size_t)6U);
            }

            /* Decrement the multicast entries. */
            hNdk2Enet->MCastCnt--;

            /* Configure the Ethernet controller with the new multicast list. */
            Ndk2Enet_setRx(hNdk2Enet);
            break;
        }

        case (unsigned int)NIMU_SET_DEVICE_MAC:
        {
            /* Validate the arguments. */
            if ((pBuf == NULL) || (size != 6U))
            {
                return -NDK_EINVAL;
            }

            break;
        }

        case (unsigned int)NIMU_GET_DEVICE_ISLINKUP:
        {
            /* Validate the arguments. */
            if (size >= sizeof(uint32_t))
            {
                if (hNdk2Enet->appInfo.isPortLinkedFxn(hNdk2Enet->appInfo.hEnet))
                {
                    *(uint32_t *)pBuf = 1;
                }
                else
                {
                    *(uint32_t *)pBuf = 0;
                }
            }
            else
            {
                /* user-provided buffer is too small */
                return -NDK_EINVAL;
            }

            break;
        }

        /* Let the driver handle rest of the commands */
        default:
        {
            /* call the switch driver's Ioctl handler to take care of this. */
            return (int)Ndk2Enet_ioctl(hNdk2Enet, cmd, pBuf, size);
        }
    }

    return 0;
}

