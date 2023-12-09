

module Settings
{
    config string timesyncVersionString = "01.00.00.00";
	
    /*! This variable is to control the device library type selection.
     * By default this variable is set to release.
     * 
     * To use timesync to use the debug/release library, add the following lines to config
     * file and set the library profile accordingly:
     * 
     *      var timesyncSettings = xdc.useModule ('ti.transport.timesync.Settings');
     *      timesyncSettings.libProfile = "debug";
     * 
     */
    metaonly config string libProfile = "release";	
}

