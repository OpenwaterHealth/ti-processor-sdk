/*
@ @licstart  The following is the entire license notice for the
JavaScript code in this file.

Copyright (C) 1997-2017 by Dimitri van Heesch

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

@licend  The above is the entire license notice
for the JavaScript code in this file
*/
var NAVTREE =
[
  [ "PDK API Guide for J721E", "index.html", [
    [ "PDK API Guide", "index.html", null ],
    [ "Enet Integration Guide", "enet_integration_guide_top.html", [
      [ "Getting Familiar with Enet LLD APIs", "enet_integration_guide_top.html#GettingFamiliarWithAPIs", [
        [ "IOCTL Interface", "enet_integration_guide_top.html#IOCTL_description", null ]
      ] ],
      [ "Integrating Enet LLD into User's Application", "enet_integration_guide_top.html#enet_integration_in_app", [
        [ "Init Sequence", "enet_integration_guide_top.html#enet_init_sequence", null ],
        [ "Peripheral Open Sequence", "enet_integration_guide_top.html#enet_open_sequence", null ],
        [ "Port Open Sequence", "enet_integration_guide_top.html#enet_openport_sequence", null ],
        [ "Packet Send/Receive Sequence", "enet_integration_guide_top.html#enet_pktrxtx_sequence", null ],
        [ "Packet Timestamping Sequence", "enet_integration_guide_top.html#enet_pktts_sequence", null ],
        [ "IOCTL Sequence", "enet_integration_guide_top.html#enet_ioctl_sequence", null ],
        [ "Port Close Sequence", "enet_integration_guide_top.html#enet_closeport_sequence", null ],
        [ "Peripheral Close Sequence", "enet_integration_guide_top.html#enet_close_sequence", null ],
        [ "Deinit Sequence", "enet_integration_guide_top.html#enet_deinit_sequence", null ],
        [ "Peripheral-specific", "enet_integration_guide_top.html#enetper_specific_handling", [
          [ "ICSSG Dual-MAC", "enet_integration_guide_top.html#enetper_icssg_dmac_specific_handling", [
            [ "Firmware", "enet_integration_guide_top.html#enetper_icssg_dmac_fw_specific_handling", null ],
            [ "Firmware Memory Pools", "enet_integration_guide_top.html#enetper_icssg_dmac_fwmem_specific_handling", null ]
          ] ],
          [ "ICSSG Switch", "enet_integration_guide_top.html#enetper_icssg_swt_specific_handling", [
            [ "Firmware", "enet_integration_guide_top.html#enetper_icssg_swt_fw_specific_handling", null ],
            [ "Firmware Memory Pools", "enet_integration_guide_top.html#enetper_icssg_swt_fwmem_specific_handling", null ]
          ] ]
        ] ]
      ] ],
      [ "Migrating from EMAC LLD", "enet_integration_guide_top.html#emac_lld_migration", [
        [ "ICSSG Migration", "enet_integration_guide_top.html#emac_lld_migration_icssg", [
          [ "Peripheral open", "enet_integration_guide_top.html#emac_lld_migration_icssg_per_open", null ],
          [ "IOCTLs", "enet_integration_guide_top.html#emac_lld_migration_icssg_ioctl", [
            [ "Asynchronous IOCTLs", "enet_integration_guide_top.html#emac_lld_migration_icssg_async_ioctl", null ],
            [ "IOCTL Command Mappings", "enet_integration_guide_top.html#emac_lld_migration_icssg_ioctl_mappings", null ]
          ] ],
          [ "Packet send and receive", "enet_integration_guide_top.html#emac_lld_migration_icssg_dma", null ]
        ] ]
      ] ]
    ] ],
    [ "Enet LLD IOCTL interface", "enet_ioctl_interface.html", [
      [ "Introduction", "enet_ioctl_interface.html#enet_ioctl_intro", [
        [ "Using the IOCTL interface", "enet_ioctl_interface.html#using_enet_ioctl", null ],
        [ "Synchronous and Asynchronous IOCTLs", "enet_ioctl_interface.html#enet_async_ioctl", null ]
      ] ]
    ] ],
    [ "Enet LLD Introduction", "enetlld_top.html", [
      [ "Introduction", "enetlld_top.html#enetlld_intro", null ],
      [ "Application Programming Interface", "enetlld_top.html#enetlld_api_overview", null ],
      [ "Enet Peripherals", "enetlld_top.html#enetlld_enetpers", [
        [ "CPSW Peripheral", "enetlld_top.html#enetper_cpsw", [
          [ "CPSW as a replacement to External Switch", "enetlld_top.html#cpsw_external_switch_replacement", null ]
        ] ],
        [ "ICSSG Peripheral", "enetlld_top.html#enetper_icssg", [
          [ "ICSSG Dual-MAC", "enetlld_top.html#enetper_icssg_dualmac", null ],
          [ "ICSSG Switch Mode", "enetlld_top.html#enetper_icssg_switch", null ]
        ] ]
      ] ],
      [ "Integration", "enetlld_top.html#enetlld_integration", null ],
      [ "Document Revision History", "enetlld_top.html#enetlld_hist", null ]
    ] ],
    [ "Ethernet PHY Integration Guide", "enetphy_guide_top.html", [
      [ "Introduction", "enetphy_guide_top.html#enetphy_guide_intro", null ],
      [ "PHY Driver", "enetphy_guide_top.html#enetphy_guide_driver", [
        [ "Device-Specific Drivers", "enetphy_guide_top.html#enetphy_guide_device_specific", null ],
        [ "PHY to Driver Binding", "enetphy_guide_top.html#enetphy_guide_binding", null ]
      ] ],
      [ "Implementing a New PHY Driver", "enetphy_guide_top.html#enetphy_guide_implementing", null ],
      [ "Appendix", "enetphy_guide_top.html#enetphy_guide_appendix", [
        [ "Appendix A", "enetphy_guide_top.html#enetphy_guide_appendix_a", null ]
      ] ]
    ] ],
    [ "Ethernet PHY Link Configuration", "enetphy_link_config_top.html", [
      [ "Link Configuration Guidelines", "enetphy_link_config_top.html#enetphy_link_config_guidelines", [
        [ "Manual Mode", "enetphy_link_config_top.html#enetphy_link_manual", [
          [ "Half-Duplex Mode", "enetphy_link_config_top.html#enetphy_link_manual_half_duplex", null ],
          [ "Full-Duplex Mode", "enetphy_link_config_top.html#enetphy_link_manual_full_duplex", null ]
        ] ],
        [ "Auto-Negotiation Mode", "enetphy_link_config_top.html#enetphy_link_autoneg", null ],
        [ "Strapping", "enetphy_link_config_top.html#enetphy_link_strapping", null ]
      ] ]
    ] ],
    [ "Modules", "modules.html", "modules" ]
  ] ]
];

var NAVTREEINDEX =
[
"enet_integration_guide_top.html",
"group__BOARD__LIB__DEVICES__FPD__DS90UB924__TYPES.html#ga1b846948e5f2e1b5315a2ca379c2407b",
"group__BOARD__LIB__DEVICES__FPD__DS90UB941__APIS.html#ga570a924ef2340cf0aa72f48398a576f3",
"group__BOARD__LIB__DEVICES__FPD__DS90UB941__TYPES.html#gafd42c0008f4a02756b813801831f71fb",
"group__BOARD__LIB__DEVICES__POWER__TPS65941__TYPES.html#ga54ac927b767f4f03d0f22f70e7dc58f3",
"group__BOARD__LIB__DEVICES__POWER__TPS65941__TYPES.html#gac94a7822748b4e4a3009f121e1e58d2c",
"group__BOARD__LIB__MODULE__TYPES.html#ga63c12ca5f1f86bc7db0d2cf1bde46abc",
"group__CPSW__ALE__MOD.html#ggabed7e8ccaf9e220220ce26e85697cb98af4c00affa8ce22df46aec0bdb0ab99c8",
"group__CSL__CPGMAC__SL__FUNCTION.html#gaf05899926eb669dbdd0c555a0b461aa6",
"group__CSL__CPSWITCH__FUNCTION.html#ga9a6e0be8ea9d1dfe772ebd6a95fc2127",
"group__CSL__DRU__ENUM.html#ga61811d2b16e6f45d7a70c332dcb8196d",
"group__CSL__DSSVIDEOPORT.html#gabc3dd9f60ec30639140c94cc296fbbcd",
"group__CSL__UDMAP__CPPI5__DATASTRUCT.html#gacfc8cf9845b89c104d630ff3e2c03043",
"group__CSL__UDMAP__FUNCTION.html#ga96cb09f28d974eacd4e8ae9e01bf4195",
"group__DRV__CSIRX__EVENT__MODULE.html#ga867746a0ff7d653a8f42015d18e78418",
"group__DRV__DSS__DCTRL__IOCTL.html#ga48fd0f5f1e1476ef03b203ae9b1ccfaa",
"group__DRV__ENET__ICSSG.html#ga0159b18ef747aa2b4dc49ea478ed76db",
"group__DRV__FCP__MODULE__CFG.html#gac0fb2fb5aa9b1ac18d555f175791f823",
"group__DRV__IPC__SOC__MODULE.html#gacce04aa57148c10fe0fb6985eb7459b9",
"group__DRV__MSC__MODULE__CFG.html#gaa4e81a6ab2678a49182a653969c86abd",
"group__DRV__OSAL__HwiP.html#gaa90fc3fb4a933440de43b075f228ba15",
"group__DRV__PMIC__API__MODULE.html#gae12f7591d2a72b7ab870a7d2129eefb1",
"group__DRV__PMIC__IRQ__MODULE.html#ga2950ad6c0acafafa241a2adeb89ab6b6",
"group__DRV__PMIC__POWER__MODULE.html#ga4c5f4dd3722186c4cbbdf7fe5ee9d3d2",
"group__DRV__PMIC__POWER__MODULE.html#gafc26cf730695e45d830a3b01b46ae40c",
"group__DRV__SDE__MODULE__INTERFACE.html#ga2339899de76b43336890e0cd856c4322",
"group__DRV__UDMA__CFG__MODULE.html#gafc434e7b6aab6b7500bff0ae898f8834",
"group__DRV__UDMA__UTILS__MODULE.html#ga1b4758d5c11bc0b695f5120e6963fb0b",
"group__ENET__MAIN__API.html#gae64f7d37ccbfa627315eb5bb76794dd9",
"group__ENET__PKTUTILS__API.html#ggae0668f1495e7a0382b12802e67a34f4aae3b3dcd4a98ce229e0a2530def12add8",
"group__SCICLIENT__FMW__RM__IF.html#ga8867730c6de0bddbf249c1accab36c6c",
"group__tisci__boardcfg__macros.html#gaec25728d9c78e1325b08e9ee20f926a9",
"group__tisci__protocol.html#gaffbc4ab20a616d08b0234bd8592abd40",
"structBoard__Tps65941RtcTime.html#aab0167ce5ef30362336013884ded6665",
"structCSITX__StreamConfig__s.html",
"structCSL__CPSW__PORTSTAT.html",
"structCSL__DruCapabilities.html#a6b1b894708687cc990df0c2b9b2d0be6",
"structCSL__UdmapRevision.html#af4029e57b77b8e5d3a1672489c97a633",
"structCpswAle__PortMirroringCfg.html",
"structCpswStats__HostPort__Ng.html",
"structCpsw__SetTxShortIpgCfgInArgs.html",
"structDss__DispPipeCropParams.html",
"structEnetRm__AllocRxFlow.html#ac9ed8efcaaa608c7ce0f95b28e10e66a",
"structFcp__CfaConfig.html#a0674a7107c1925bd6a68cd38b681566a",
"structIcssgStats__MacPort.html#a45a44a6b35737b467371a6e0d75f7fba",
"structLdc__Config.html#a55ff59e987c4c4db30f01af2cebd849d",
"structNsf4__PwlConfig.html#a8f6692712321eb405e93beab02f51aab",
"structRfe__PwlConfig.html#a073864bc6aa2b88450f72382b654d675",
"structUART__Params.html#a4849aaa2dfa2a010f2361e14b77841d7",
"structUdma__InitPrms.html#a9b83624c1bb1022671ac1b6de7197725",
"structVhwa__M2mVissParams.html#a867b1ec4f4cb6908d1487b9e1b762fa8",
"structtisci__msg__get__clock__parent__req.html#a660d791e6b8b870647b6ae47db40384b",
"structtisci__msg__rm__psil__read__req.html#a6632072230edcc8bc0475a71ea55e9b6",
"structtisci__msg__version__req.html#a9893c933e29bcd14262c5622cd4c1409"
];

var SYNCONMSG = 'click to disable panel synchronisation';
var SYNCOFFMSG = 'click to enable panel synchronisation';