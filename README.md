# LTC2977-Linduino

Software to communicate with the LTC2977 chip from Allied Technology throuh the Linduino.

The Linduino is a special arduino wit a shield made by Allied Technology

This software is an custom implementation of the PMBUS without PEC using arduino'wire library. In fact we only use only 
I2C to communicate and don't implement the features of the PMBUS.

There are two parts, one that goes on the Linduino microcontroller and a python code to send command to the
LTC through the microcontroller.


## Arduino
The arduino sketch needs to be uploaded to the arduino. For this we need the arduino IDE. And add the 
library needed by the sketch.

Start the arduino ide and configure it to talk to the board. In the menu **Tools** and with the linduino on Linux

    Board: Arduino Uno
    port: /dev/ttyUSB0

On windows you need to find the COM port.

If you want to use the sketch within the arduino ide don't forget to configure the serial monitor to use 115200 baud 

### library
To install the library LTC2977_Linduino.cpp  we need to start the arduino ide, open the LTC_2977_Linduino.ino
sketch and go to the menu

    Sketch -> Include Library -> Add .ZIP library

Then navigate to the folder containing the library here we go to **Arduino/libraries/LTC2977_duino/** 
and then click Ok. The library will be copied in **~/Arduino/libraries/LTC2977_duino/** and you need to overwrite the files
there if you make modification.

#### upgrade library

The  library path seems to be set to the first location it was set. To find why you cannot use a modified library you need
to 

    Select File > Preferences from the Arduino IDE's menus.
    Check the box next to "Show verbose output during: compilation" 

It will help to find the location of the folder for the library.

See   https://forum.arduino.cc/t/removing-a-library-solved-it/285012/3 for some ideas.

### push the sketch
Using the arduino ide click on the arrow to upload the sketch. Don't forget to configure the connection to the board 
through the **Tools** menu. 

Once this is done we can use the python script to communicate with the LTC.


## Python

To install the python module you need to install the module, preferably in a virtualenv, using:

    python ./setup.py install



