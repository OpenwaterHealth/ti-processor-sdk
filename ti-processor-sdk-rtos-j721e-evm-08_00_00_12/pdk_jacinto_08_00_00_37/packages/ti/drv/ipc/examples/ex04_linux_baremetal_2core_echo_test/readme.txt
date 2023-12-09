Details of the ex04_linux_baremetal_2core_echo_test

Main test Files
../common/src/main_baremetal.c          : This file has the main function for the application
../common/src/ipc_testsetup_baremetal.c : This file has the echo test function
ex04_linux_baremetal_2core_echo_test.c  : This file has the test configuration
../common/src/r5f_mpu_am65xx_default.c  : This file has the override MPU configuration for the R5F appliction

Other files
../common/src/ipc_am65xx_rsctable.h     : This file has the resource table used by the test application
../common/src/ipc_utils.h,.c            : This file has some ipc utility functions
../common/src/ipc_trace.h,.c            : This file has functions to support the trace functionality, which allows to see the trace
                                          from the linux host

Functional description
----------------------

The ex04_linux_baremetal_2core_echo_test mainly initializes an rpmsg receive node and sends an announcment message to advertize the receive node.
After initialization the application mainly executes the responder function which waits for a "ping" message and sends a "pong" message back
to the source.


       ----------                 ---------
       |        |     Ping        |        |
       |        |---------------->|        |
       | Linux  |                 | R5F    |
       | Host   |     Pong        | Echo   |
       |        |<----------------| test   |
       |        |                 |        |
       ----------                 ----------
