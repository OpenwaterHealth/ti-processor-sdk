

module Settings
{
    config string dssVersionString = "01.00.00.00";

    /*! This variable is to control the SoC type selection.
     * By default this variable is set to NULL.
     * 
     * To use DSS for the selected device, add the following lines to config
     * file and set the deviceType correctly:
     *
     *      var dssSettings = xdc.useModule ('ti.drv.dss.Settings');
     *      dssSettings.socType = "am65xx";
     * 
     */
    metaonly config string socType = "";
    
    /*! This variable is to control the Board type selection.
     * By default this variable is set to NULL.
     * 
     * To use DSS for the selected device, add the following lines to config
     * file and set the deviceType correctly:
     *
     *      var dssSettings = xdc.useModule ('ti.drv.dss.Settings');
     *      dssSettings.boardType = "am65xx_evm";
     * 
     */
    metaonly config string boardType = "";

    /*! This variable is to control the device library type selection.
     * By default this variable is set to release.
     * 
     * To use the debug/release library, add the following lines to config
     * file and set the library profile accordingly:
     * 
     *      var dssSettings = xdc.useModule ('ti.drv.dss.Settings');
     *      dssSettings.libProfile = "debug";
     * 
     */
    metaonly config string libProfile = "release";
}
