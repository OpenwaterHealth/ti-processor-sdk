/*
 * helloWorld_bios6.c
 *
 * TCP/IP Stack 'Hello World!' Example ported to use BIOS6 OS.
 *
 * Copyright (C) 2007, 2011 Texas Instruments Incorporated - http://www.ti.com/
 *
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
 *
*/

//--------------------------------------------------------------------------
// IP Stack 'Hello World!' Example
//
// This is a skeleton application, intended to provide application
// programmers with a basic Stack setup, to which they can start
// adding their code.
//
// To test it as is, use with helloWorld.exe from \winapps directory
//

#include <stdio.h>


#include <xdc/std.h>
#include <xdc/runtime/IHeap.h>
#include <xdc/runtime/System.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/Memory.h>
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/heaps/HeapBuf.h>
#include <ti/sysbios/heaps/HeapMem.h>

#ifdef _TMS320C6X
#include <ti/csl/csl_chipAux.h>
#endif

#include <ti/csl/csl_chip.h>
#include <ti/csl/csl_pscAux.h>

#ifdef __ARM_ARCH_7A__
#include <ti/sysbios/family/arm/a15/Mmu.h>
#include <ti/csl/cslr_msmc.h>
#include <ti/csl/csl_pscAux.h>
#include <ti/csl/csl_bootcfgAux.h>
#endif
/* BIOS6 include */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>

#ifdef _TMS320C6X
#include <ti/sysbios/family/c64p/Hwi.h>
#include <ti/sysbios/family/c64p/EventCombiner.h>
#else
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/family/arm/a15/Mmu.h>
#endif


#include <ti/ndk/inc/netmain.h>
#include <ti/transport/ndk/nimu/example/helloWorld/src/nimu_cppi_qmss_iface.h>

#include <ti/drv/emac/emac_drv.h>
#include <ti/drv/emac/soc/emac_soc_v1.h>


#include <ti/drv/uart/UART.h>
#include <ti/drv/uart/UART_stdio.h>
#include <ti/board/board.h>

#define NIMU_NUM_HOST_DESC 128
#ifdef _TMS320C6X
#pragma DATA_SECTION(gHostDesc, ".resmgr_memregion");
#pragma DATA_ALIGN (gHostDesc, 128)
uint8_t                                 gHostDesc[1 * NIMU_NUM_HOST_DESC];
#else
uint8_t      gHostDesc[EMAC_SIZE_HOST_DESC * NIMU_NUM_HOST_DESC] __attribute__ ((aligned (128)));

#endif

bool useDhcp = TRUE;

//---------------------------------------------------------------------------
// Title String
//
char *VerStr = "\nTCP/IP Stack 'Hello World!' Application\n\n";

// Our NETCTRL callback functions
void   myNetworkOpen();
void   myNetworkClose();
void   myNetworkIpAddr( uint32_t IPAddr, uint32_t IfIdx, uint32_t fAdd );

// Fun reporting function
static void   ServiceReport( uint32_t Item, uint32_t Status, uint32_t Report, void* hCfgEntry );

// External references
extern int dtask_udp_hello();

//---------------------------------------------------------------------------
// Configuration
//
char *HostName    = "tidsp";
char *LocalIPAddr = "192.168.1.4";
char *LocalIPMask = "255.255.255.0";    // Not used when using DHCP
char *GatewayIP   = "192.168.1.1";    // Not used when using DHCP
char *DomainName  = "demo.net";         // Not used when using DHCP
char *DNSServer   = "0.0.0.0";          // Used when set to anything but zero


//---------------------------------------------------------------------
// Main Entry Point
//---------------------------------------------------------------------
int main()
{

    EMAC_HwAttrs_V1 emac_cfg;
    Board_STATUS boardInitStatus =0;
	
#ifdef __ARM_ARCH_7A__

    /* Add MMU entries for MMR's required for example */
    Uint32 privid, index;
    CSL_MsmcRegs *msmc = (CSL_MsmcRegs *)CSL_MSMC_CFG_REGS;
    Mmu_DescriptorAttrs attrs;
    extern char ti_sysbios_family_arm_a15_Mmu_Module_State_0_secondLevelTableBuf_1__A;
    uint32_t addr = (uint32_t)&ti_sysbios_family_arm_a15_Mmu_Module_State_0_secondLevelTableBuf_1__A;

    Mmu_initDescAttrs(&attrs);

    attrs.type = Mmu_DescriptorType_TABLE;
    attrs.shareable = 0;            // non-shareable
    attrs.accPerm = 1;              // read/write at any privelege level
    attrs.attrIndx = 0;             // Use MAIR0 Register Byte 3 for
                                    // determining the memory attributes
                                    // for each MMU entry


    // Update the first level table's MMU entry for 0x80000000 with the
    // new attributes.
    Mmu_setFirstLevelDesc((Ptr)0x40000000, (UInt64)addr, &attrs);

    // Set up SES & SMS to make all masters coherent
    for (privid = 0; privid < 16; privid++)
    {
      for (index = 0; index < 8; index++)
      {
        uint32_t ses_mpaxh = msmc->SES_MPAX_PER_PRIVID[privid].SES[index].MPAXH;
        uint32_t sms_mpaxh = msmc->SMS_MPAX_PER_PRIVID[privid].SMS[index].MPAXH;
        if (CSL_FEXT (ses_mpaxh, MSMC_SES_MPAXH_0_SEGSZ) != 0)
        {
          // Clear the "US" bit to make coherent.  This is at 0x80.
          ses_mpaxh &= ~0x80;
          msmc->SES_MPAX_PER_PRIVID[privid].SES[index].MPAXH = ses_mpaxh;
        }
        if (CSL_FEXT (sms_mpaxh, MSMC_SMS_MPAXH_0_SEGSZ) != 0)
        {
          // Clear the "US" bit to make coherent.  This is at 0x80.
          sms_mpaxh &= ~0x80;
          msmc->SMS_MPAX_PER_PRIVID[privid].SMS[index].MPAXH = sms_mpaxh;
        }
      }
    }
#endif

    Board_initCfg cfg = BOARD_INIT_UART_STDIO | BOARD_INIT_PINMUX_CONFIG | BOARD_INIT_MODULE_CLOCK | BOARD_INIT_ETH_PHY ;
    boardInitStatus = Board_init(cfg);
    if (boardInitStatus !=0)
    {
        UART_printf("Board_init failure\n");
        return(0);
    }
    UART_printf("Board_init success\n");

    nssPowerUp();

    EMAC_socGetInitCfg(0, &emac_cfg);
    /* Need to update the descriptor base address */
    emac_cfg.p_desc_base = &gHostDesc;
#ifdef ICE_K2G
    emac_cfg.phyAddr = BOARD_GIGABIT_EMAC_PHY_ADDR;
#endif
    /* Now set the config after updating desc base address */
    EMAC_socSetInitCfg(0, &emac_cfg);
	
	/* Start the BIOS 6 Scheduler */
    BIOS_start ();
    
    return 0;
}



/***************************************************************************************
 * FUNCTION PURPOSE: Power up PA subsystem
 ***************************************************************************************
 * DESCRIPTION: this function powers up the PA subsystem domains
 ***************************************************************************************/
void nssPowerUp (void)
{
    /* PASS power domain is turned OFF by default. It needs to be turned on before doing any 
     * PASS device register access. This not required for the simulator. */

    /* Set NSS Power domain to ON */        
    CSL_PSC_enablePowerDomain (CSL_PSC_PD_NSS);

    /* Enable the clocks for NSS modules */
    CSL_PSC_setModuleNextState (CSL_PSC_LPSC_NSS, PSC_MODSTATE_ENABLE);

    /* Start the state transition */
    CSL_PSC_startStateTransition (CSL_PSC_PD_NSS);

    /* Wait until the state transition process is completed. */
    while (!CSL_PSC_isStateTransitionDone (CSL_PSC_PD_NSS));
}

//
// Main Thread
//
void stackInitHook(void* hCfg)
{
    int             rc;

    rc = 16384; // increase stack size
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_TASKSTKBOOT,CFG_ADDMODE_UNIQUE, sizeof(uint32_t), (uint8_t *)&rc, 0 );

    // We better validate the length of the supplied names
    if( strlen( DomainName ) >= CFG_DOMAIN_MAX ||
        strlen( HostName ) >= CFG_HOSTNAME_MAX )
    {
        platform_write("Names too long\n");
    }

    // Add our global hostname to hCfg (to be claimed in all connected domains)
    CfgAddEntry( hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_HOSTNAME, 0,
                 strlen(HostName), (uint8_t *)HostName, 0 );

    // If the IP address is specified, manually configure IP and Gateway
    if (!useDhcp)
    {
        CI_IPNET NA;
        CI_ROUTE RT;
        uint32_t      IPTmp;

        platform_write("StackTest: using localIp\n");
        // Setup manual IP address
        bzero( &NA, sizeof(NA) );
        NA.IPAddr  = inet_addr(LocalIPAddr);
        NA.IPMask  = inet_addr(LocalIPMask);
        strcpy( NA.Domain, DomainName );
        NA.NetType = 0;

        // Add the address to interface 1
        CfgAddEntry( hCfg, CFGTAG_IPNET, 1, 0,
                           sizeof(CI_IPNET), (uint8_t *)&NA, 0 );

        // Add the default gateway. Since it is the default, the
        // destination address and mask are both zero (we go ahead
        // and show the assignment for clarity).
        bzero( &RT, sizeof(RT) );
        RT.IPDestAddr = 0;
        RT.IPDestMask = 0;
        RT.IPGateAddr = inet_addr(GatewayIP);

        // Add the route
        CfgAddEntry( hCfg, CFGTAG_ROUTE, 0, 0,
                           sizeof(CI_ROUTE), (uint8_t *)&RT, 0 );

        // Manually add the DNS server when specified
        IPTmp = inet_addr(DNSServer);
        if( IPTmp )
            CfgAddEntry( hCfg, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER,
                         0, sizeof(IPTmp), (uint8_t *)&IPTmp, 0 );
    }
    // Else we specify DHCP
    else
    {
        platform_write("StackTest: using dhcp\n");
        CI_SERVICE_DHCPC dhcpc;

        // Specify DHCP Service on IF-1
        bzero( &dhcpc, sizeof(dhcpc) );
        dhcpc.cisargs.Mode   = CIS_FLG_IFIDXVALID;
        dhcpc.cisargs.IfIdx  = 1;
        dhcpc.cisargs.pCbSrv = &ServiceReport;
        CfgAddEntry( hCfg, CFGTAG_SERVICE, CFGITEM_SERVICE_DHCPCLIENT, 0,
                     sizeof(dhcpc), (uint8_t *)&dhcpc, 0 );
    }

    //
    // Configure IPStack/OS Options
    //

    // We don't want to see debug messages less than WARNINGS
    rc = DBG_WARN;
    CfgAddEntry( hCfg, CFGTAG_OS, CFGITEM_OS_DBGPRINTLEVEL,
                 CFG_ADDMODE_UNIQUE, sizeof(uint32_t), (uint8_t *)&rc, 0 );

    //
    // This code sets up the TCP and UDP buffer sizes
    // (Note 8192 is actually the default. This code is here to
    // illustrate how the buffer and limit sizes are configured.)
    //

    // UDP Receive limit
    rc = 8192;
    CfgAddEntry( hCfg, CFGTAG_IP, CFGITEM_IP_SOCKUDPRXLIMIT,
                 CFG_ADDMODE_UNIQUE, sizeof(uint32_t), (uint8_t *)&rc, 0 );

    rc = 4096; // increase stack size
    CfgAddEntry(hCfg, CFGTAG_OS, CFGITEM_OS_TASKSTKBOOT,
    CFG_ADDMODE_UNIQUE, sizeof(uint32_t), (uint8_t *)&rc, 0 );

}


//
// System Task Code [ Server Daemon Servers ]
//
static void* hHello=0;

//
// NetworkOpen
//
// This function is called after the configuration has booted
//
void myNetworkOpen()
{
    if (!useDhcp)
    {
        UART_printf("\n\rSYS/BIOS HelloWorld (CPSW) Sample application, EVM IP address I/F 0: %s\n\r", LocalIPAddr);
    }
    // Create our local server
    hHello = DaemonNew( SOCK_DGRAM, 0, 7, dtask_udp_hello,
                       OS_TASKPRINORM, OS_TASKSTKNORM, 0, 1 );
}

//
// NetworkClose
//
// This function is called when the network is shutting down,
// or when it no longer has any IP addresses assigned to it.
//
void myNetworkClose()
{
    DaemonFree( hHello );
}


//
// NetworkIPAddr
//
// This function is called whenever an IP address binding is
// added or removed from the system.
//
void myNetworkIpAddr( uint32_t IPAddr, uint32_t IfIdx, uint32_t fAdd )
{
    uint32_t IPTmp;

    if( fAdd )
        printf("Network Added: ");
    else
        printf("Network Removed: ");

    // Print a message
    IPTmp = NDK_ntohl( IPAddr );
    printf("If-%d:%d.%d.%d.%d\n", IfIdx,
            (uint8_t)(IPTmp>>24)&0xFF, (uint8_t)(IPTmp>>16)&0xFF,
            (uint8_t)(IPTmp>>8)&0xFF, (uint8_t)IPTmp&0xFF );

    UART_printf("If-%d:%d.%d.%d.%d\n", IfIdx,
            (uint8_t)(IPTmp>>24)&0xFF, (uint8_t)(IPTmp>>16)&0xFF,
            (uint8_t)(IPTmp>>8)&0xFF, (uint8_t)IPTmp&0xFF );
}

//
// Service Status Reports
//
// Here's a quick example of using service status updates
//
static char *TaskName[]  = { "Telnet","HTTP","NAT","DHCPS","DHCPC","DNS" };
static char *ReportStr[] = { "","Running","Updated","Complete","Fault" };
static char *StatusStr[] = { "Disabled","Waiting","IPTerm","Failed","Enabled" };
static void ServiceReport( uint32_t Item, uint32_t Status, uint32_t Report, void* h )
{
    printf( "Service Status: %-9s: %-9s: %-9s: %03d\n",
            TaskName[Item-1], StatusStr[Status],
            ReportStr[Report/256], Report&0xFF );

    //
    // Example of adding to the DHCP configuration space
    //
    // When using the DHCP client, the client has full control over access
    // to the first 256 entries in the CFGTAG_SYSINFO space.
    //
    // Note that the DHCP client will erase all CFGTAG_SYSINFO tags except
    // CFGITEM_DHCP_HOSTNAME. If the application needs to keep manual
    // entries in the DHCP tag range, then the code to maintain them should
    // be placed here.
    //
    // Here, we want to manually add a DNS server to the configuration, but
    // we can only do it once DHCP has finished its programming.
    //
    if( Item == CFGITEM_SERVICE_DHCPCLIENT &&
        Status == CIS_SRV_STATUS_ENABLED &&
        (Report == (NETTOOLS_STAT_RUNNING|DHCPCODE_IPADD) ||
         Report == (NETTOOLS_STAT_RUNNING|DHCPCODE_IPRENEW)) )
    {
        uint32_t IPTmp;

        // Manually add the DNS server when specified
        IPTmp = inet_addr(DNSServer);
        if( IPTmp )
            CfgAddEntry( 0, CFGTAG_SYSINFO, CFGITEM_DHCP_DOMAINNAMESERVER,
                         0, sizeof(IPTmp), (uint8_t *)&IPTmp, 0 );
    }
}

