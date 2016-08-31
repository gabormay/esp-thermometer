The objective of this project is to create a really inexpensive internet enabled thermometer to be able to monitor a room's temperature remotely.

# Main components
Here's what you're going to need:
- ESP-01, a ESP8266 chip based WiFi module (others may also work with some modifications)
- DS18S20 digital thermometer chip (or compatible)
- 2xAA batteries or a 3V power adapter
- FTDI or USB-to-Serial (3v3!) converter (for programming the ESP8266)
- some cables, and a few discrete components (see the schematic)
- lot of patience :)

# General notes and caveats
- ESP8266 works with 3.3V, the pins are _not_ 5V tolerant!
- Use an external power supply for reliable programming (not the 3.3V from the USB converter for example)
- By now, there's extensive documentation is available for the ESP8266 and related stuff. If you're stuck or have an issue, pleaes  try looking through the materials in the [References](References and further reading) section and also try Google - chances are somebody else had that problem and you can find a solution quickly

# First steps
Start with the installation steps from https://github.com/esp8266/Arduino#installing-with-boards-manager
- Install the Arduino IDE 
- Install the ESP8266 core library and select the board called 'Generic ESP8266 Module'

Wire up your ESP module on a breadboard as explained in https://github.com/esp8266/Arduino/blob/master/doc/boards.md#minimal-hardware-setup-for-bootloading-and-usage
- Use an external power source (do not use the USB-to-serial adapter's VCC)
- GND, TX and RX should be connected to the serial adapter's GND, RX and TX, respectively
- CH_PD (chip enable) should be connected the VCC (10k pull-up resistor recommended)
- RST and GPIO0 should be pulled-up to VCC
  * For convenience, add a switch or push button to be able to pull each down to GND momentarily
- Leave GPIO2 unconnected

Now load the Sketch from File/Examples/ESP8266/Blink
- Fill in your WiFi ssid and password
- 'Verify' should show no errors

Now you're ready to flash you first program to the ESP8266! Make sure that you have the Board setting in Arduino IDE set correctly:
![Board settings in Arduino IDE](img/Board settings.png)


# Building the thermometer
## The circuit
## The software

# Running from a battery

# What else?
[TBD: things to improve / try]

# References and further reading

