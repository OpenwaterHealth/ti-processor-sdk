/*
 * Copyright (c) 2017-2020, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

//
//File Name: launch_am65xx.js
//Description:
//   Launch the DMSC firmware and board configuration from R5F.
//
//Usage:
//
//From CCS Scripting console
//  1. loadJSFile "C:\\ti\\launch_am65xx.js"
//     <Ignore the error observed on the CCS scripting console
//     "Wrapped java.io.IOException: The handle is invalid (<location_of_script>\
//     launch_am65xx.js#93)">
//
//Note:
//  1. Search for "edit this" to look at changes that need to be edited
//     for your usage.
//


//<!!!!!! EDIT THIS !!!!!>
// Set this to 1 to allow loading the GEL files directly from the ccxml file.
disableGelLoad = 1;
if (disableGelLoad == 0)
{
    //Path to GEL files
    gelFilePath = "/ti/ccs910/ccs/ccs_base/emulation/boards/am65x/gel";
}
// Path to the directory in which this file would be residing. CCS expects
// absolute paths to load the binaries.
thisJsFileDirectory = "/ti/j7presi/workarea/pdk/packages/ti/drv/sciclient/tools/ccsLoadDmsc/am65xx/";
//<!!!!!! EDIT THIS !!!!!>

// Import the DSS packages into our namespace to save on typing
importPackage(Packages.com.ti.debug.engine.scripting)
importPackage(Packages.com.ti.ccstudio.scripting.environment)
importPackage(Packages.java.lang)
importPackage(java.io);
importPackage(java.lang);

function updateScriptVars()
{
    //Open a debug session
    dsMCU1_0 = debugServer.openSession( ".*MCU_PULSAR_Cortex_R5_0" );
    dsDMSC_0 = debugServer.openSession( ".*DMSC_Cortex_M3_0" );
}

function printVars()
{
    updateScriptVars();
}

function connectTargets()
{
    /* Set timeout of 20 seconds */
    script.setScriptTimeout(60000);
    updateScriptVars();
    sysResetVar=dsDMSC_0.target.getResetType(1);
    sysResetVar.issueReset();
    print("Connecting to DMSC_Cortex_M3_0!");
    if (disableGelLoad == 0)
    {
        // Load the GEL. This can be removed if the GEL is already linked with the target ccxml
        dsDMSC_0.expression.evaluate('GEL_LoadGel("'+gelFilePath+'/AM65xEVM.gel")');
    }
    // Connect targets
    dsDMSC_0.target.connect();
    dsMCU1_0.target.connect();

    // Read the Device ID to load the correct SYSFW
    dev_id = dsMCU1_0.memory.readWord(0, 0x43000014); // CSL_WKUP_CTRL_MMR0_CFG0_BASE + CSL_WKUP_CTRL_MMR_CFG0_JTAGID

    print("Loading DMSC Firmware...");
    // Load the DMSC firmware
    if (dev_id == 0x0BB5A02F) {
        dsDMSC_0.memory.loadRaw(0, 0x40000, thisJsFileDirectory +"/../../../soc/sysfw/binaries/ti-sci-firmware-am65x-gp.bin", 32, false);
    } else if (dev_id == 0x1BB5A02F) {
        dsDMSC_0.memory.loadRaw(0, 0x40000, thisJsFileDirectory +"/../../../soc/sysfw/binaries/ti-sci-firmware-am65x_sr2-gp.bin", 32, false);
    } else {
        print("Invalid Device ID!");
        return;
    }

    print("DMSC Firmware Load Done...");
    // Set Stack pointer and Program Counter
    stackPointer = dsDMSC_0.memory.readWord(0, 0x40000);
    progCounter = dsDMSC_0.memory.readWord(0, 0x40004);
    dsDMSC_0.memory.writeRegister("SP", stackPointer);
    dsDMSC_0.memory.writeRegister("PC", progCounter);
    print( "DMSC Firmware run starting now...");
    // Run the DMSC firmware
    dsDMSC_0.target.runAsynch();
    print("Connecting to MCU Cortex_R5_0!");

    // Connect the MCU R5F
    dsMCU1_0.target.connect();
    if (disableGelLoad == 0)
    {
        // Load the GEL. This can be removed if the GEL is already linked with the target ccxml
        dsMCU1_0.expression.evaluate('GEL_LoadGel("'+gelFilePath+'/M4_DDR39SS/M4_R5orA53_Startup.gel")');
    }
    print("Running the board configuration initialization from R5!");
    // Load the board configuration init file.
    dsMCU1_0.memory.loadProgram(thisJsFileDirectory +"/sciclient_ccs_init_mcu1_0_release.xer5f");
    // Halt the R5F and re-run.
    dsMCU1_0.target.halt();
    dsMCU1_0.target.reset();
    dsMCU1_0.target.restart();
    // Run Synchronously for the executable to finish
    dsMCU1_0.target.run();
}

function disconnectTargets()
{
    updateScriptVars();

    // Disconnect targets
    dsDMSC_0.target.disconnect();
    // Reset the R5F to be in clean state.
    dsMCU1_0.target.reset();
    print("Initializing DDR!");
    // Execute DDR initialization script from R5F.
    try
    {
        dsMCU1_0.expression.evaluate("DDR4_800MHz_Initialization_for_EVM()");
        dsMCU1_0.expression.evaluate("timer0_cleanup()");
    }
    catch(e)
    {
        print("Some error in GEL execution for DDR4_CONFIG");
    }
}

function sampleDDRCheck ()
{
    print("Running DDR Memory Checks....");
    dsMCU1_0.memory.fill (0x80000000, 0, 1024, 0xA5A5A5A5);
    ar = dsMCU1_0.memory.readWord(0, 0x80000000, 1024);
    fail = 0 
    for (i = 0; i < ar.length; i++) {
            x = ar[i]; 
            if (x != 0xA5A5A5A5)
            {   
                fail = 1;
            }
        } 
    if (fail == 1)
    {   
        print ("0x80000000: DDR memory sample check failed !!");
    }
    dsMCU1_0.memory.fill (0x81000000, 0, 1024, 0x5A5A5A5A);
    ar = dsMCU1_0.memory.readWord(0, 0x81000000, 1024);
    fail = 0 
    for (i = 0; i < ar.length; i++) {
            x = ar[i]; 
            if (x != 0x5a5a5a5a)
            {   
                fail = 1;
            }
        } 
    if (fail == 1)
    {   
        print ("0x81000000: DDR memory sample check failed !!");
    }

}


function doEverything()
{
    printVars();
    connectTargets();
    disconnectTargets();
    sampleDDRCheck ();
    print("Happy Debugging!!");
}

var ds;
var debugServer;
var script;

// Check to see if running from within CCSv4 Scripting Console
var withinCCS = (ds !== undefined);

// Create scripting environment and get debug server if running standalone
if (!withinCCS)
{
    // Import the DSS packages into our namespace to save on typing
    importPackage(Packages.com.ti.debug.engine.scripting);
    importPackage(Packages.com.ti.ccstudio.scripting.environment);
    importPackage(Packages.java.lang);

    // Create our scripting environment object - which is the main entry point into any script and
    // the factory for creating other Scriptable ervers and Sessions
    script = ScriptingEnvironment.instance();

    // Get the Debug Server and start a Debug Session
    debugServer = script.getServer("DebugServer.1");
}
else // otherwise leverage existing scripting environment and debug server
{
    debugServer = ds;
    script = env;
}

doEverything();

