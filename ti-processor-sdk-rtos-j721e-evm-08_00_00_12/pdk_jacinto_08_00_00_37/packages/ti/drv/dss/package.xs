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
    var suffix  = prog.build.target.suffix;
    var socType = this.Settings.socType;
    var boardType = this.Settings.boardType;
    var socTypes = [
                     'am65xx',
                   ];
    var boardTypes = [
                     'am65xx_sim',
                     'am65xx_evm'
                   ];
    var libNames = [
                     'dss'
                   ];

    /*
     * The same suffix "r5f" is used for both R5f arm and thumb library
     */
    suffix = java.lang.String(suffix).replace('r5ft','r5f');

    /* Read LIBDIR variable */
    var lib = java.lang.System.getenv("LIBDIR");

    /* If NULL, default to "lib" folder */
    if (lib == null)
    {
        lib = "./lib";
    } else {
        print ("\tSystem environment LIBDIR variable defined : " + lib);
    }

    /* Get the Board */
    for each (var board in boardTypes)
    {
        if (boardType.equals(board))
        {
            lib = lib + "/" + board;
            name = this.$name + ".a" + suffix;
            break;
        }
    }

    /* Get target folder, if applicable */
    if (java.lang.String(suffix).contains('mcu1_0'))
        lib = lib + "/mcu1_0";
    else if (java.lang.String(suffix).contains('mcu2_0'))
        lib = lib + "/mcu2_0";
    else if (java.lang.String(suffix).contains('mpu1_1'))
        lib = lib + "/mpu1_1";
    else if (java.lang.String(suffix).contains('mpu1_0'))
        lib = lib + "/mpu1_0";
    else if (java.lang.String(suffix).contains('a53'))
        lib = lib + "/mpu1_0";
    else if (java.lang.String(suffix).contains('r5f'))
        lib = lib + "/mcu1_0";
    else
        throw new Error("\tUnknown target for: " + this.packageBase + lib);

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

    /* Update the lib names with the lib extension */
    lib_dir = lib;
    lib     ="";
    for each(var libName in libNames)
    {
        libName = libName + ".a" + suffix;
        if ((java.io.File(this.packageBase + lib_dir + "/" + libName).exists()))
        {
            /* Get library name with path */
            lib = lib + lib_dir +"/" + libName;
            lib = lib + ";";
            print ("\tLinking with library " + this.packageBase + lib_dir + "/" + libName );
        }
        else
        {
           /* Could not find any library, throw exception */
           throw new Error("\tLibrary not found: " + this.packageBase + lib_dir + "/" + libName);
           break;
        }
    }

    /* Get library name with path */
    return lib;
}

/*
 *  ======== package.init ========
 */
function init() {
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
