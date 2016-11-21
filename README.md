The objective of this project is to create a really inexpensive internet enabled thermometer to be able to monitor a room's temperature remotely.

  ![Completed project photo 1](img/final-1.jpg)
  ![Completed project photo 2](img/final-2.jpg)
  
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
- If you're stuck or have an issue with setting up the ESP8266, please try looking through the materials in the [References](References and further reading) section and also try Google - chances are somebody else had that problem and you can find a solution quickly

# First steps - programming your ESP8266
Start with the installation steps from https://github.com/esp8266/Arduino#installing-with-boards-manager:
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

Then put the ESP bootloader into programming mode:
- Open the Serial Monitor in Arduino IDE (Tools/Serial Monitor)
  * Choose the proper COM port
  * Choose 74880 baud (this is necessary in order to see bootloader messages properly)
- On the board, connect both `RST` and `GPIO0` to GND (push the buttons if you have added them)
- Release RST (connect back to VCC), while `GPIO0` is still at GND
- In the Serial Monitor you should see the following message from the bootloader:

    ```
     ets Jan  8 2013,rst cause:2, boot mode:(1,6)
    ```
    This means that the module is now in serial programming mode and ready to receive the new firmware.
- `GPIO0` can now be released back to VCC
- Select Sketch/Upload

In a short while the upload should finish without errors. If everything went fine the blue LED on the board will start blinking. You can now reset the board by connecting `RST` to GND momentarily. Make sure that `GPIO0` is _not_ connected to GND otherwise you will enter the programming mode again.

At this point I suggest you to play around with your ESP8266. Try changing the example or try other sketches. Look at the the list of functions in the [included libraries](https://github.com/esp8266/Arduino/blob/master/doc/libraries.md) and try to use them in your program.

You can use the `Serial.print()` family of functions to print diagnostic messages that you can then read in the Serial Monitor. Set the baud rate to 74880 to make it consistent with the bootloader via `Serial.begin(74880)`.

Once you feel comfortable programming your ESP8266, come back here and read on.

# Building the thermometer
## The circuit

   ![Schematic](img/schematic.png)

Explanation:
- U1 is a 2x4 DIP header for connecting the ESP module
- SERIAL is a 1x3 DIP header for connecting a serial interface
  * Connect GND, TX and RX to your USB-toSerial adapter's GND, RX and TX, respectively
  * Double-check that the adapter uses 3.3V signal levels (not 5v)
- RST and PRG are push buttons, activating RESET and Programming mode, respectively
  * to simply reset the board, push RST
  * to enter programming mode 
    + push RST then PRG (keep both pushed)
    + Release RST then PRG
- C2 is across the rails, try to put it close to the module power pins (increases stability of the board)
- R1 and C1 is an RC delay circuit for reliable deep sleep operation
  * purpose is to keep CH_PD low for enough time for the board to properly reset upon waking from deep sleep
  * only applies if your GPIO16 is tied to CH_PD
- U2 is the digital thermometer chip (DS18S20)
- R2 is a pull-up

## Ubidots
- Sign up at [Ubidots](https://ubidots.com/)
- Create a source with two variables:
  * Temperature (unit: C)
  * Battery voltage (unit: mV)
- Note your API token and the ID of these two variables

## The software
Prerequisites
- Install the [DS1820 Arduino Library] 
- Install the [OneWire Arduino Library]

Download and edit the [source](src/ESPThermometer/ESPThermometer.ino)
- Fill in you SSID and WiFi credentials
- Fill in your Ubidots token and variable IDs (from above)

Connect your circuit to the serial adapter. Make sure the serial interface is connected to the USB-to-serial adapter properly (see above) and the adapter is plugged in to a USB port on your computer

Connect the power rails to an external 3.3V power source (e.g. 2xAA battery or a stabilized wall adapter)
- In general anything between 2.7 - 3.3 V should work
- Do **not** use the serial adapter's VCC, as it cannot provide the amount of current necessary for programming

Once you have everything connected, you should download the program to the module
- Put the module into program mode (push RST, PRG and then release RST, PRG)
- Verify that you get the correct bootloader prompt - see [](First steps - programming your ESP8266)
- Load the program into the Arduino IDE and upload (make sure to use the same settings as in [](First steps - programming your ESP8266)
- Reset the board for good measure (push and release RST)

If everything went well you should see an output similar to this in the serial monitor:

```
 ets Jan  8 2013,rst cause:2, boot mode:(3,6)

load 0x4010f000, len 1384, room 16 
tail 8
chksum 0x2d
csum 0x2d
v3ffe8654
~ld
à
ESP Thermometer starting...
Chip ID: [<your chip ID here>]

VCC = 2581 mV
Requesting temperatures...DONE
TEMP = 26.6 C
Connecting and sending...
.......WiFi connected
IP address: 
192.168.178.44
Posting your variables
HTTP/1.1 200 OK
Server: nginx
Date: Wed, 31 Aug 2016 20:18:29 GMT
Content-Type: application/json
Transfer-Encoding: chunked
Connection: close
Vary: Accept-Encoding
Vary: Accept
Allow: POST, OPTIONS

2c
[{"status_code": 201}, {"status_code": 201}]
0

Entering deep sleep [ 557s ] ...

 ets Jan  8 2013,rst cause:1, boot mode:(3,6)

[... and so on ...]

```

# Running from a battery
I was able to run this project from 2 AA batteries for about 2-3 weeks - not great but not that bad either. Interestingly enough the thermometer chip proved to be more sensitive to loss of voltage. It stopped working and produced bogus measurements when the battery voltage fell below 2.4V or so. The ESP module worked fine until about 1.8V, which is pretty remarkable.

Anyway, let's dig a little bit into how this was made possible and what can you do to further improve battery life.

## Deep sleep
Deep sleep is a power saving feature of the ESP8266 chip. When put to deep sleep, only an internal RTC circutry is powered, drawing very tiny current (~20uA). The rest of the chip is powered down and is completely inactive (no program runs for example). Once the specified delay is over, the RTC puts out a signal on GPIO16 which can be used to wake up the chip. Normally you would want to connect GPIO16 to RST to get the most features. On some ESP boards it is tied to CH_PD (chip enable) which works too but some of the features, i.e. keeping some state between sleep cycles is not supported. See the Application Note [ESP Low Power Solutions] from Expressif for more info. 

Note that, in any case, upon wake-up, your program (sketch) will run from the very beginning (starting with `setup()`), so you don't really need to put anything in `loop()` at all. See the Application Note mentioned above for options to save some variables between these sleep cycles.

## Modifications to the board
### Scrapping the Power LED

The onboard power LED draws quite a bit of current (few milliamps at least) all the time the board is powered, even during deep sleep. That is almost 1000 times the current the chip needs in deep sleep, for just a plain power indicator, which is not a critical feature at all. On the ESP-01 board you don't have any control over this, so your only option to get rid of this 'powet hog' is to scrape the LED physically from the board. Be gentle, but firm. See [Running ESP8266 from a battery] for details.

### Connecting GPIO 16

To be able to wake the chip up from deep sleep, GPIO need to be connected to CH_PD or RST. In case it is not connected on your board, you will have to do it yourself. See http://tim.jagenberg.info/2015/01/18/low-power-esp8266/

## Doing even better
Here are a few things you can do to increase the battery life of your project even further.

* Use higher capacity power supply
  * You can use higher capacity batteries (e.g. D or A)
  * You can also use higher voltage batteries e.g. 3 x AA (or A or D) and add a 3V power regulator
* Update less frequently (less often than every 10 minutes)
  * Currently the project set up to wake up and update every ten minutes. 
  * You can change the length of the sleep cycle on line [#31](https://github.com/gabormay/esp-thermometer/blob/master/src/ESPThermometer/ESPThermometer.ino#L31)
* Proper deep sleep (GPIO16 to RST) 
  * If you connect GPIO16 to RST then you can keep some state betweeen cycles. 
  * In particular, you can set it up to do RF calibration less frequently
  * You can also collect multiple measurements (e.g. every minute) then transmit them together as a batch

# What's next?
There are a lot of ways to further improve and extend this project. Here are a few ideas and pointers to get you started.

- Dynamic WiFi setup
  Instead of hardcoding the SSID and password, provide a simple Web interface to connect to the WiFi initially. Note that this will only work with proper deep sleep (GPIO16 to RST) as you would want to save this information betweeen the sleep cycles.
- Dynamic Ubidots variable setup based on Chip ID
  You can create the variables on the fly, e.g. using the chip ID (first check if they exist, and create them if not)
- Implement additional hardware functions:
  * connect a second sensor, e.g. for humidity or light (GPIO2 is still free)
  * add relay switch and control it through Ubidots (again, GPIO2 can be used)
  * replace Ubidots with some other (maybe custom) IoT platform of your choice

# Closing words

Hope you've found this useful. If you run into problems and could not find anything on the internet and in the referenced documents, let me know - I will try to help as much as my time allows. In any case, please feel free to use the information presented in this article in any way you wish (no warranty, though, please refer to the LICENSE). Good luck!

# References and further reading
Reference
- [Vendor docs](http://bbs.espressif.com/)
- [Community ESP Wiki](http://www.esp8266.com/wiki/doku.php)
- [Arduino Core for ESP8266](https://github.com/esp8266/Arduino)
- [Some useful info here](https://github.com/esp8266/esp8266-wiki/wiki)

Tutorials/guides
- http://blog.brianmoses.net/2015/07/figuring-out-the-esp8266.html
- http://williamdurand.fr/2015/03/17/playing-with-a-esp8266-wifi-module/
- http://rancidbacon.com/files/kiwicon8/ESP8266_WiFi_Module_Quick_Start_Guide_v_1.0.4.pdf

Deep sleep/battery operation
- [Running ESP8266 from a battery]
- ["RC time-delay circuit is recommended for CH_EN"](http://bbs.espressif.com/viewtopic.php?t=646)
- [ESP Low Power Solutions]

DS1820
- [DS1820 Data sheet]
- [DS1820 Arduino Library]
  - https://github.com/milesburton/Arduino-Temperature-Control-Library
- [OneWire Arduino Library]

[DS1820 Data sheet]: http://www.produktinfo.conrad.com/datenblaetter/175000-199999/176168-da-01-en-TEMP_SENSOR_DS18S20_TO92_MAXIM_DALLAS_.pdf
[DS1820 Arduino Library]: http://milesburton.com/Main_Page?title=Dallas_Temperature_Control_Library
[OneWire Arduino Library]: https://github.com/PaulStoffregen/OneWire
[ESP Low Power Solutions]: http://www.espressif.com/sites/default/files/9b-esp8266-low_power_solutions_en_0.pdf
[Running ESP8266 from a battery]: https://www.openhomeautomation.net/esp8266-battery/

