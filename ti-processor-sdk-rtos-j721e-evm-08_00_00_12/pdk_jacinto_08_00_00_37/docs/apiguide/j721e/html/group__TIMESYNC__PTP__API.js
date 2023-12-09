var group__TIMESYNC__PTP__API =
[
    [ "TimeSyncPtp_TimeStamp", "structTimeSyncPtp__TimeStamp.html", [
      [ "seconds", "structTimeSyncPtp__TimeStamp.html#ae68fad816c5fbe26f7b51f0081fc8e87", null ],
      [ "nanoseconds", "structTimeSyncPtp__TimeStamp.html#a44245f9a5b4c49a842e219187aa45cdd", null ]
    ] ],
    [ "TimeSyncPtp_ExtendedTimestamp", "structTimeSyncPtp__ExtendedTimestamp.html", [
      [ "seconds", "structTimeSyncPtp__ExtendedTimestamp.html#a63c655fb1adf091de1f2cc090d73966a", null ],
      [ "fractionalNanoseconds", "structTimeSyncPtp__ExtendedTimestamp.html#a75bdd587f7105f8cf6147e5473fc9635", null ]
    ] ],
    [ "TimeSyncPtp_MasterParams", "structTimeSyncPtp__MasterParams.html", [
      [ "priority1", "structTimeSyncPtp__MasterParams.html#a9aa0f57584efcb05a435917694aaad98", null ],
      [ "priority2", "structTimeSyncPtp__MasterParams.html#a8bc989c44fa67568085510a35f41ae60", null ],
      [ "clockClass", "structTimeSyncPtp__MasterParams.html#a01b3df316755523236704b2b7c27b7ca", null ],
      [ "clockAccuracy", "structTimeSyncPtp__MasterParams.html#a4a4ba7a71bc6c702159e3a53a22f739c", null ],
      [ "timeSource", "structTimeSyncPtp__MasterParams.html#a1438ebbb67ba7fb09a4a1c406d7c9fa3", null ],
      [ "stepRemoved", "structTimeSyncPtp__MasterParams.html#ade8cd6c37ee63dab077e920afdbbeb12", null ],
      [ "clockVariance", "structTimeSyncPtp__MasterParams.html#adec2358abc5e805d29f4ed25e332a4dd", null ],
      [ "UTCOffset", "structTimeSyncPtp__MasterParams.html#a0f7bd5c522b64af73fb29294c5cd0d58", null ],
      [ "gmIdentity", "structTimeSyncPtp__MasterParams.html#a75445972ee4d912a08e4e70d542268b1", null ],
      [ "ptpFlags", "structTimeSyncPtp__MasterParams.html#afe132e913ac70a41e9f9bde498ed888f", null ]
    ] ],
    [ "TimeSyncPtp_Config", "structTimeSyncPtp__Config.html", [
      [ "nwDrvHandle", "structTimeSyncPtp__Config.html#adaa216813bf0d0619cef1823bb3d3e71", null ],
      [ "socConfig", "structTimeSyncPtp__Config.html#aedca87ec04ba793d080f298b4c8a7274", null ],
      [ "deviceMode", "structTimeSyncPtp__Config.html#aca279ff27e74b28d5b771ec1da28a7a3", null ],
      [ "type", "structTimeSyncPtp__Config.html#a7e97d8aae95ddd002ed6479475ec7868", null ],
      [ "protocol", "structTimeSyncPtp__Config.html#aed95e3539d9b91efc7cb4b52e8f44df9", null ],
      [ "vlanCfg", "structTimeSyncPtp__Config.html#af0a2fa2875a07c8e7687d269e3be7627", null ],
      [ "isMaster", "structTimeSyncPtp__Config.html#a0bed63fa08bbfbaa87dfb925dd00738e", null ],
      [ "masterParams", "structTimeSyncPtp__Config.html#aedd9094c96a5d706132b1898f2bb1351", null ],
      [ "portMask", "structTimeSyncPtp__Config.html#a267da8e543e0c7baca3b01198122f63b", null ],
      [ "syncLossNotifyFxn", "structTimeSyncPtp__Config.html#a6898674c457da2e78aaae7143572747c", null ],
      [ "asymmetryCorrection", "structTimeSyncPtp__Config.html#ae7f9f834fbd5cde70c1d75a4265882d6", null ],
      [ "domainNumber", "structTimeSyncPtp__Config.html#a0d3153b2d448f2f34a0c9db28a1dc76a", null ],
      [ "frameOffset", "structTimeSyncPtp__Config.html#a7323be1aed9c1bd681a45c50ec95c5a4", null ],
      [ "tickPeriod", "structTimeSyncPtp__Config.html#aac9542efa961c84929ccb64748b036f8", null ],
      [ "logPDelReqPktInterval", "structTimeSyncPtp__Config.html#ad70c7bf36080132299f8a8ab74d708af", null ],
      [ "logSyncInterval", "structTimeSyncPtp__Config.html#a14fd554d5db3bf3b4f6edb392829f42c", null ],
      [ "logAnnounceSendInterval", "structTimeSyncPtp__Config.html#a99412c8b09f0597d2f0592981e996a30", null ],
      [ "logAnnounceRcptTimeoutInterval", "structTimeSyncPtp__Config.html#abc4c317e4a8406ff06517fbfda0169bb", null ],
      [ "pDelReqPktInterval", "structTimeSyncPtp__Config.html#a236be3746f35a23f86355453bc5b4ea3", null ],
      [ "syncSendInterval", "structTimeSyncPtp__Config.html#a2327b9a3421ebd87376c413ed84e8e28", null ],
      [ "announceSendInterval", "structTimeSyncPtp__Config.html#a6c1fb3a1eb10957899b5f76584e7cd69", null ],
      [ "announceRcptTimeoutInterval", "structTimeSyncPtp__Config.html#a94f04a00550d551f15b9c021126a8ae2", null ],
      [ "pdelayBurstInterval", "structTimeSyncPtp__Config.html#aa7ff9861ceee1f9f4a86d1613ee806c0", null ],
      [ "pdelayBurstNumPkts", "structTimeSyncPtp__Config.html#ab52c8be1b7147b8ed3d661429c3792ce", null ],
      [ "ppsPulseIntervalNs", "structTimeSyncPtp__Config.html#a89d578f162cda71d3ae20efd9f35ccc0", null ],
      [ "ifMacID", "structTimeSyncPtp__Config.html#a9780e3f88b35c942a7afe87b3aed4836", null ],
      [ "ipAddr", "structTimeSyncPtp__Config.html#a638bb78491b64df2e54097bf83a1d3e9", null ],
      [ "hsrEnabled", "structTimeSyncPtp__Config.html#a2ee91f087abdce097e8c5555bcc050b0", null ],
      [ "processToDSync2PPS", "structTimeSyncPtp__Config.html#a851577d12267b6df5a94a305ceb38954", null ],
      [ "ll_has_hsrTag", "structTimeSyncPtp__Config.html#a5c27dcc8390cd5b7fa53ad5cf1600182", null ]
    ] ],
    [ "TIMESYNC_PTP_MAX_PORTS_SUPPORTED", "group__TIMESYNC__PTP__API.html#ga7e8cab589b5677c25ad80c5f7278c096", null ],
    [ "TimeSyncPtp_SyncLossNotify", "group__TIMESYNC__PTP__API.html#gaef51a63fc5704b88d72c8933e3a7c52f", null ],
    [ "TimeSyncPtp_Handle", "group__TIMESYNC__PTP__API.html#gacc37be94c839d48c8f2fadfc14123f85", null ],
    [ "TimeSyncPtp_DelayType", "group__TIMESYNC__PTP__API.html#gae45ed062877677c66d407836c85e7116", [
      [ "TIMESYNC_PTP_DELAY_E2E", "group__TIMESYNC__PTP__API.html#ggae45ed062877677c66d407836c85e7116a70f394e58df59fb8fed8754b490b10c9", null ],
      [ "TIMESYNC_PTP_DELAY_P2P", "group__TIMESYNC__PTP__API.html#ggae45ed062877677c66d407836c85e7116a567086ffdbd73ce30e58105aec416569", null ]
    ] ],
    [ "TimeSyncPtp_TimeSource", "group__TIMESYNC__PTP__API.html#ga3dbe864c868d6ee3a7fb480150462bb9", [
      [ "TIMESYNC_CLKSRC_ATOMIC_CLOCK", "group__TIMESYNC__PTP__API.html#gga3dbe864c868d6ee3a7fb480150462bb9a3ab7bc87a95b3ffb2229ddca38f4ec3e", null ],
      [ "TIMESYNC_CLKSRC_GPS", "group__TIMESYNC__PTP__API.html#gga3dbe864c868d6ee3a7fb480150462bb9a8c88f3c4be818b9c946237be18ee4a07", null ],
      [ "TIMESYNC_CLKSRC_TERRESTRIAL_RADIO", "group__TIMESYNC__PTP__API.html#gga3dbe864c868d6ee3a7fb480150462bb9aac9f6f181099dbb20d735ed54ecebfcf", null ],
      [ "TIMESYNC_CLKSRC_PTP", "group__TIMESYNC__PTP__API.html#gga3dbe864c868d6ee3a7fb480150462bb9a123f711f767b098eecc2ed99074bd423", null ],
      [ "TIMESYNC_CLKSRC_NTP", "group__TIMESYNC__PTP__API.html#gga3dbe864c868d6ee3a7fb480150462bb9a6baccd113220aae9b81eb04945980923", null ],
      [ "TIMESYNC_CLKSRC_HAND_SET", "group__TIMESYNC__PTP__API.html#gga3dbe864c868d6ee3a7fb480150462bb9a580c35d50f90225ea5a19727fed0d93c", null ],
      [ "TIMESYNC_CLKSRC_OTHER", "group__TIMESYNC__PTP__API.html#gga3dbe864c868d6ee3a7fb480150462bb9ad05fd73383aa16b62724907becacbab3", null ],
      [ "TIMESYNC_CLKSRC_INTERNAL_OSCILLATOR", "group__TIMESYNC__PTP__API.html#gga3dbe864c868d6ee3a7fb480150462bb9abd6060db22885ac135e6174b88503b2c", null ]
    ] ],
    [ "TimeSyncPtp_setDefaultPtpConfig", "group__TIMESYNC__PTP__API.html#ga292ee449fa881c912980515dc448bcc0", null ],
    [ "TimeSyncPtp_init", "group__TIMESYNC__PTP__API.html#ga81d0dd02b22a2b97c05947731daa9226", null ],
    [ "TimeSyncPtp_deInit", "group__TIMESYNC__PTP__API.html#ga85bb2fc7f44c9b5d29f919c73b8442c2", null ],
    [ "TimeSyncPtp_enable", "group__TIMESYNC__PTP__API.html#ga9d68645336b80cb35ee3ff8a77168241", null ],
    [ "TimeSyncPtp_disable", "group__TIMESYNC__PTP__API.html#ga30ee2e8c0ff03f70530bf747a85fa069", null ],
    [ "TimeSyncPtp_PortLinkResetCallBack", "group__TIMESYNC__PTP__API.html#ga4d8ae7fc535dc99576a072e4e8239f15", null ],
    [ "TimeSyncPtp_reset", "group__TIMESYNC__PTP__API.html#gac7f54743d643328be9044fa07147d8fa", null ],
    [ "TimeSyncPtp_isEnabled", "group__TIMESYNC__PTP__API.html#ga1a4818b544b31c20b79d04ad5599c40d", null ],
    [ "TimeSyncPtp_enableMaster", "group__TIMESYNC__PTP__API.html#ga4a8ed49201b6ee6021554be7fe583ff2", null ],
    [ "TimeSyncPtp_disableMaster", "group__TIMESYNC__PTP__API.html#gad7de5eced27f908d77833f844f851de9", null ]
];