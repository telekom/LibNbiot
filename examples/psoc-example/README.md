# NBIoTLibPSoC5Example - An example how to use the NB-IoT library on the Cypress CY8CKIT-059 PSoC 5LP Prototyping Kit

The folder contains the compressed workspace bundle **NBIoTLibPSoC5Example.cywrk.Archive.zip**. After unpacking, the
contained workspace can be opened with the **Cypress PSoC Creator 4.0** or above. The **PSoC Creator** allows to build
the example and to programm it to the PSoC 5LP device via the **KitProg Onboard Programmer and Debugger**.

## Hardware Prerequisites

The workspace is created for the Cypress CY8CKIT-059 PSoC 5LP Prototyping Kit. If using another Kit it might be
necessary to change project setup and the pinout in **PSoC Creator**. 

Connect the UART TX/RX pins of a compatible NB-IoT modem to the UART RX/TX (12.6/12.7) pins of the Prototyping Kit.
The modem has to be equipped with a SIM card which is registered for the Telekom Cloud of Things.
Use the USB port of the Prototyping Kit for monitoring the device with a terminal (e.g. PuTTY) It might be necessary 
to install an USB-Serial driver to your system. The Cypress USB-Serial driver for Windows is part of the package.

## Software Prerequisites

The binaries of `LibNbiot` and `LibNbiotCore` need to be available before building the example.

Edit the toplevel makefile to compile the libraries for `arm cortex m3` (GCC cross arm compiler is needed):

    export PLATFORMS := arm-cortex-m3 arm-cortex-m3-debug

Build the libraries by calling **make** at the toplevel.
Copy the resulting binaries to the corresponding folders in PSoC5 example:

    lib/arm-cortex-m3/libmqttsnpaho.a       to NBIoTLibPSoC5Example/LibNbiot/release/
    lib/arm-cortex-m3/libstm.a              to NBIoTLibPSoC5Example/LibNbiot/release/
    lib/arm-cortex-m3/libnbiot.a            to NBIoTLibPSoC5Example/LibNbiot/release/
    lib/arm-cortex-m3/libnbiotcore.a        to NBIoTLibPSoC5Example/LibNbiotCore/release/
    lib/arm-cortex-m3-debug/libmqttsnpaho.a to NBIoTLibPSoC5Example/LibNbiot/debug/
    lib/arm-cortex-m3-debug/libstm.a        to NBIoTLibPSoC5Example/LibNbiot/debug/
    lib/arm-cortex-m3-debug/libnbiot.a      to NBIoTLibPSoC5Example/LibNbiot/debug/
    lib/arm-cortex-m3-debug/libnbiotcore.a  to NBIoTLibPSoC5Example/LibNbiotCore/debug/


## Building the example

Open the workspace file **NBIoTLibPSoC5Example.cywrk** with the **PSoC Creator**. Edit the file `auth.h`:

    #define AUTH_IMSI "111111111111111"
    #define AUTH_PWD "PASSWORD"

Replace `111111111111111` by a 15-digit IMSI which is registered for the Telekom Cloud of Things and
replace `PASSWORD` by the 8-characters password distributed with IMSI for Telekom Cloud of Things access.

Build the PSoC 5LP firmware from within the **PSoC Creator** and programm it via the USB connection to
the **KitProg Onboard Programmer and Debugger**.
The output can be monitored with a terminal connected to the Prototyping Kits USB port.
