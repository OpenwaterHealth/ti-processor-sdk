

module Settings
{
    config string uartVersionString = "01.00.00.16";
    /*! This variable is to control the SoC type selection.
     * By default this variable is set to NULL.
     * 
     * To use UART for the selected device, add the following lines to config
     * file and set the deviceType correctly:
	 *
     *      var uartSettings = xdc.useModule ('ti.drv.uart.Settings');
     *      uartSettings.socType = "am572x";
     * 
     */
    metaonly config string socType = "";
    /*! This variable is to control the DMA type selection.
     * By default this variable is set to false.
     * 
     * To use DMA SoC LIbrary for the selected device, add the following lines to config
     * file and set the useSocDma correctly:
	 *
     *      var uartSettings = xdc.useModule ('ti.drv.uart.Settings');
     *      uartSettings.useDma = "true";
     * 
     */
    metaonly config string useDma = "false";	
    /*! This flag is used to indicate whether or not the benchmarking code
     * (defined in the profilingHooks class) will be used in the project.
     * Note that a separate library has been compiled and will be used
     * ($NAME).profiling.a($SUFFIX). This is set in the *.cfg file.
     */
    config Bool enableProfiling = false;
	
    /*! This variable is to control the device library type selection.
     * By default this variable is set to release.
     * 
     * To use CSL to use the debug/release library, add the following lines to config
     * file and set the library profile accordingly:
     * 
     *      var Uart Settings = xdc.useModule ('ti.Uart.Settings');
     *      UartSettings.libProfile = "debug";
     * 
     */
    metaonly config string libProfile = "release";	

    /*! This variable controls whether or not UART SW IP (PRU firmware) will be 
     * used in the project.
     * 
     * By default this variable is set to false.
     * 
     * To use UART SW IP for the selected device, add the following lines to config
     * file and set the enableSwIp correctly:
	 *
     *      var uartSettings = xdc.useModule ('ti.drv.uart.Settings');
     *      uartSettings.enableSwIp = true;
     * 
     */
    config Bool enableSwIp = false;
}
