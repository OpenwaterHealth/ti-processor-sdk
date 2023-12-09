

module Settings
{
    config string sciclientVersionString = "01.00.00.04";

    /*! This variable is to control the SoC type selection.
     * By default this variable is set to NULL.
     * 
     * To use SCICLIENT for the selected device, add the following lines to config
     * file and set the deviceType correctly:
     *
     *      var sciclientSettings = xdc.useModule ('ti.drv.sciclient.Settings');
     *      sciclientSettings.socType = "am65xx";
     * 
     */
    metaonly config string socType = "";
    
    /*! This variable is to control the Board type selection.
     * By default this variable is set to NULL.
     * 
     * To use SCICLIENT for the selected device, add the following lines to config
     * file and set the deviceType correctly:
     *
     *      var sciclientSettings = xdc.useModule ('ti.drv.sciclient.Settings');
     *      sciclientSettings.boardType = "am65xx_evm";
     * 
     */
    metaonly config string boardType = "";

    /*! This variable is to control the core type selection.
     * By default this variable is set to NULL.
     *
     * To use SCICLIENT for the selected core, add the following lines to config
     * file and set the deviceType correctly:
     *
     *      var sciclientSettings = xdc.useModule ('ti.drv.sciclient.Settings');
     *      sciclientSettings.coreType = "mcu1_0";
     *
     */
    metaonly config string coreType = "";

    /*! This variable is to control the device library type selection.
     * By default this variable is set to release.
     * 
     * To use the debug/release library, add the following lines to config
     * file and set the library profile accordingly:
     * 
     *      var sciclientSettings = xdc.useModule ('ti.drv.sciclient.Settings');
     *      sciclientSettings.libProfile = "debug";
     * 
     */
    metaonly config string libProfile = "release";
}
