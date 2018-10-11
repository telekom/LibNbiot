# Arduino Example - An example how to use the LibNbiot on an Arduino

This folder contains the python-script **create_arduino_lib.py** that creates a LibNbiot.zip that can be imported as 
a library by the Arduino IDE (version 1.8.5. required). The imported library ships with two easy-to-use example called
**ArduinoMegaExample** and **Tuino096Example**. 

## Hardware Prerequisites

So far, the library was tested succesfully on an Arduino Mega and Tuino096. Other AVR-µC as well as SAMD-µC should also work but make sure that that 
these µC have at least two hardware serial interfaces (one for communication with the modem, one for debug outputs) 
and enough SRAM.

##### ArduinoMegaExample
Connect the UART TX/RX pins of a compatible NB-IoT modem to the UART RX/TX (19,18) pins of the Arduino Mega.
##### Tuino096Example
On the Tuino096 Board the appropriate pins of the Modem are already connected to the µC.    

In addition the modem has to be equipped with a **SIM card** which is registered for the **Telekom Cloud of Things**. 

## Software Prerequisites

In additional to LibNbiot, you will need other librariries for serial communication and timer setting in order to get the examples running.

For **ArduinoMegaExample** you need:
* Serial 
* TimerOne

For **Tuino096Example** you need:
* RTCZero
* Adafruit_ZeroTimer

## Starting the example

Execute the python-script. You will get a prompt to choose the module/firmware you want to use:

    --- Available firmwares:
    ---  1: SARA-N2XX-B657SP3   
    ---  2: SARA-N2XX-B656
    ---  3: BG96MA-R02A07M1G      
    
    --- Enter firmware index: 
 
The script will create a LibNbiot[xxx].zip-library for the specified module/firmware. [xxx] is replaced by the specific module/firmware.

Open the Arduino IDE and import the newly created LibNbiot[xxx].zip-library. Open
one of the given examples: **ArduinoMegaExample**, **Tuino096Example**. Change the following Makros:

    #define AUTH_IMSI "111111111111111"
    #define AUTH_PWD "PASSWORD"

Replace `111111111111111` by a 15-digit IMSI which is registered for the Telekom Cloud of Things and
replace `PASSWORD` by the 8-characters password distributed with IMSI for Telekom Cloud Things access.

Compile the code and flash it to the Arduiono. Open the Serial Monitor to observe Debug Outputs.
