# Arduino-stat-screen
PC statistics screen using an Arduino Uno and 2.8" TFT display.

This solution is designed to work with a 2.8" TFT display for Arduino (such as the https://www.adafruit.com/product/1651) based on the Adafruit_GFX and Adafruit_TFTLCD libraries. A PC application sends the statistics data to the Arduino, equipped with the TFT display, over a USB connection. The PC application is written in Python 3 and requires glances and pyserial to be installed on the system.

# Getting Started
Hardware setup is done by mounting the TFT display shield onto the Arduino UNO. Depending on what hardware configuration is available to you, Other microcontrollers and types of displays supported by the used libraries can word as well, however, they will require changes to the microcontroller code. Display functionalitiy is based on the examples given in https://github.com/adafruit/TFTLCD-Library. The microcontroller is connected to the PC over USB in order to flash the microcontroller code and send pc stats to the display.

In order to flash the Arduino code to the microcontroller, the Adafruit_GFX and Adafruit_TFTLCD libraries need to be added to the  installed Arduino IDE (https://www.arduino.cc/en/software). A tutorial for doing so can be found at https://www.arduino.cc/en/guide/libraries. Afterwards, the sketch can be loaded and flashed (see https://www.arduino.cc/en/Guide).

# Linux Software Setup
Glances (https://nicolargo.github.io/glances/) is used for exporting the statistics to a python client using ZeroMQ. Since the PC application is based on Python 3 necessary modules have to be installed. It is recommended to use Pip for doing so (https://pypi.org/project/pip/). With Python 3 and Pip installed, the following command installs all necessary python components:

'''
pip install glances pyserial pyzmq
'''

The provided glances configuration file must be placed in '~/.config/glances/' and the current user must be added to the usb group.

After installation of software components and flashing of the microcontroller, the serial port of the connected Arduino can be declared in stat_server.py (portn) and the application can be started using 'runApplication.sh'. 

# Windows Software Setup
TODO