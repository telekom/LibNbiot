# MSP432 Example - An example how to use the NB-IoT library on the TI MSP-EXP432P401R LaunchPad

The folder contains the sourcecode and the project files to build the example with the **Code Composer Studio 7.x** (ti.com/ccs).

The example uses features of the TI-RTOS operating system and shows the capabilities of the NB-IoT library in a multithreading environment.

## Hardware Prerequisites

The project is created for the Texas Instruments MSP-EXP432P401R LaunchPad Development Kit.

Connect the UART TX/RX pins of a compatible NB-IoT modem to the UART RX/TX (P3.2/P3.3) pins of the LaunchPad Development Kit.
The modem has to be equipped with a SIM card which is registered for the Telekom Cloud of Things.
Use the USB port of the LaunchPad Development Kit for monitoring the device with a terminal (e.g. PuTTY) It might be necessary 
to install an USB-Serial driver to your system.

## Software Prerequisites

The TI-RTOS operating system need to be installed in **CCS** using the **Resource Explorer**.

It is necessary to import the project 'tirtos_builds_MSP_EXP432P401R_release_ccs' to the **CCS Workspace**. Therefore select
"Project->Import CCS Projects..." in CCS-menu. Then 

	select search-directory: "C:\ti\simplelink_msp432_sdk_1_XX_XX_XX\kernel\tirtos\builds"

and choose 'tirtos_builds_MSP_EXP432P401R_release_ccs' from the 'Discovered projects' to be imported to the workspace.

The binaries of `LibNbiot` and `LibNbiotCore` need to be available before building the example.

Edit the toplevel makefile to compile the libraries for `msp432` (TI arm compiler 18.x is needed, which can be installed via CCS):

    export PLATFORMS := msp432 msp432-debug

Build the libraries by calling **make** at the toplevel.


## Building the example

Import the project to the **Code Composer Studio**. Edit the file `auth.h`:

    #define AUTH_IMSI "111111111111111"
    #define AUTH_PWD "PASSWORD"

Replace `111111111111111` by a 15-digit IMSI which is registered for the Telekom Cloud of Things and
replace `PASSWORD` by the 8-characters password distributed with IMSI for Telekom Cloud Things access.

Build the MSP432 firmware from within the **CCS** and programm it via the USB connection with a call to

    Run->Debug

The output can be monitored with a terminal connected to the LaunchPad Development Kits USB port.
