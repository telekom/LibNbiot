# Arduino Example - An example how to use the LibNbiot on an Arduino

This folder contains the python-script **create_arduino_lib.py** that creates a LibNbiot.zip that can be imported as 
a library by the Arduino IDE (version 1.8.5. required). The imported library ships with an easy-to-us example called
**SimpleExample**. 

## Hardware Prerequisites

So far, the library was tested succesfully on an Arduino Mega. Other AVR-µC should also work but make sure that that 
these µC have at least two hardware serial interfaces (one for communication with the modem, one for debug outputs) 
and enough SRAM.

Connect the UART TX/RX pins of a compatible NB-IoT modem to the UART RX/TX (19,18) pins of the Arduino Mega.
The modem has to be equipped with a SIM card which is registered for the Telekom Cloud of Things.

## Software Prerequisites

In additional to the LibNbiot, you will need the Serial as well as the TimerOne library. TimeOne can be imported by
the library mangement tool from the Arduino IDE.

## Starting the example

Execute the python-script. Open the Arduino IDE and import the newly created LibNbiot.zip-library and open the 
example **SimpleExample**.

    #define AUTH_IMSI "111111111111111"
    #define AUTH_PWD "PASSWORD"

Replace `111111111111111` by a 15-digit IMSI which is registered for the Telekom Cloud of Things and
replace `PASSWORD` by the 8-characters password distributed with IMSI for Telekom Cloud Things access.

Compile the code and flash it to the Arduiono Mega. Open the Serial Monitor to observe Debug Outputs.
