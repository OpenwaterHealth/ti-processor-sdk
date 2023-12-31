

module Settings
{
    config string i2cVersionString = "01.00.00.16";
    /*! This variable is to control the SoC type selection.
     * By default this variable is set to NULL.
     * 
     * To use LLD for the selected device, add the following lines to config
     * file and set the deviceType correctly:
	 *
     *      var i2cSettings = xdc.useModule ('ti.drv.i2c.Settings');
     *      i2cSettings.socType = "am572x";
     * 
     */
    metaonly config string socType = "";

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
     *      var i2cSettings = xdc.useModule ('ti.drv.i2c.Settings');
     *      i2cSettings.libProfile = "debug";
     * 
     */
    metaonly config string libProfile = "release";	
    
    /*! This variable controls whether or not I2C SW IP (PRU firmware) will be 
     * used in the project.
     *
     * By default this variable is set to false.
     * 
     * To use I2C SW IP for the selected device, add the following lines to config
     * file and set the enableSwIp correctly:
	 *
     *      var i2cSettings = xdc.useModule ('ti.drv.i2c.Settings');
     *      i2cSettings.enableSwIp = true;
     * 
     */
    config Bool enableSwIp = false;
    
    /*! This flag is used to indicate whether to use the AM437x ICSS0
     * I2C LLD library.
     *
     * By default this variable is set to false.
     */
    config Bool fwIcss0 = false;
}
