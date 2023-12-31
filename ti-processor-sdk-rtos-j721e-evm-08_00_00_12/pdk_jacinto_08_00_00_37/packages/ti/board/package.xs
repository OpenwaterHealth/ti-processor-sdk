/*
 *  ======== package.xs ========
 *
 */


/*
 *  ======== Package.getLibs ========
 *  This function is called when a program's configuration files are
 *  being generated and it returns the name of a library appropriate
 *  for the program's configuration.
 */

function getLibs(prog)
{
    var suffix = prog.build.target.suffix;

    /*
     * The same suffix "r5f" is used for both R5f arm and thumb library
     */
    suffix = java.lang.String(suffix).replace('r5ft','r5f');

    var name = this.$name + ".a" + suffix;

    var board = this.Settings.boardName;

    /* Read LIBDIR variable */
    var lib = java.lang.System.getenv("LIBDIR");

    /* If NULL, default to "lib" folder */
    if (lib == null)
    {
        lib = "./lib";
    } else {
        print ("\tSystem environment LIBDIR variable defined : " + lib);
    }

    /* Get board name prefix */
    lib = lib + "/" + board;

    /* Get target folder */
    if ( java.lang.String(suffix).contains('66') )
        lib = lib + "/c66";
    else if ( java.lang.String(suffix).contains('a15') )
        lib = lib + "/a15";
    else if ( java.lang.String(suffix).contains('a8') )
        lib = lib + "/a8";
    else if ( java.lang.String(suffix).contains('a9') )
        lib = lib + "/a9";
    else if ( java.lang.String(suffix).contains('e9') )
        lib = lib + "/arm9";
	else if ( java.lang.String(suffix).contains('e674') )
        lib = lib + "/c674";
    else if ( java.lang.String(suffix).contains('m4') )
        lib = lib + "/m4";
	else if ( java.lang.String(suffix).contains('r5') )
		lib = lib + "/r5f";
	else if ( java.lang.String(suffix).contains('a53') )
		lib = lib + "/a53";

    var libProfiles = ["debug", "release"];
    /* get the configured library profile */
    for each(var profile in libProfiles)
    {
        if (this.Settings.libProfile.equals(profile))
        {
            lib = lib + "/" + profile;
            break;
        }
    }	

    /* Get library name with path */
    lib = lib + "/" + name;
    if (java.io.File(this.packageBase + lib).exists()) {
       print ("\tLinking with library " + this.$name + ":" + lib);
       return lib;
    }

    /* Could not find any library, throw exception */
    throw new Error("\tLibrary not found: " + this.packageBase + lib);
}

function init() {
xdc.loadPackage("ti.drv.uart");
xdc.loadPackage("ti.drv.i2c");
xdc.loadPackage("ti.osal");
xdc.loadPackage("ti.csl");
}

/*
 *  ======== package.close ========
 */
function close()
{    
    if (xdc.om.$name != 'cfg') {
        return;
    }
}

