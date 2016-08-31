/*
 * WiFi enabled battery operated thermometer
 * 
 * ESP8266-01 (standalone) + DS18S20 temp sensor + Ubidots
 * 
 * (c) gabormay
 * http://gabor.maylander.net/esp-thermometer/
 */
#include <ESP8266WiFi.h>

// From https://github.com/PaulStoffregen/OneWire
#include <OneWire.h>

// From https://github.com/milesburton/Arduino-Temperature-Control-Library
#include <DallasTemperature.h>

// From https://ubidots.com/docs/devices/ESP8266.html
#include <UbidotsMicroESP8266.h>

// Fill in your wifi connection details
#define SSID "..."
#define PASS "..."

// Fill in your Ubidots API key (token)
#define TOKEN "..."
// ID of Battery voltage variable
#define ID_METER1_BAT_VOLTAGE "..."
// ID of temperature variable
#define ID_METER1_TEMPERATURE "..."

#define MEASURE_CYCLE_MS 1000*60*10 // 10 minutes

// needed for ESP.getVcc()
ADC_MODE(ADC_VCC);

// the DS1820 is connected to GPIO0
#define ONE_WIRE_BUS 0
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

Ubidots client(TOKEN);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  // Use 74880 baud to see bootloader messages properly
  Serial.begin(74880);
  
  Serial.println();
  Serial.println("ESP Thermometer starting...");
  // We could use the chip ID to distinguish between multiple instances of the Thermometer
  Serial.printf("Chip ID: [%x]\n\n", ESP.getChipId());
  delay(10);

  // Station mode
  WiFi.mode(WIFI_STA);

  // initialize the thermo sensor
  sensors.begin();

  acquireAndSendData();
  
  // try to calibrate sleep cycle so that we get measurements at the desired regular intervals
  unsigned long sleepMs = MEASURE_CYCLE_MS;
  unsigned long elapsedMs = millis();
  // sanity check, elapsedMs can be big if you don't reset after upload...
  if (sleepMs > elapsedMs * 5)
    sleepMs -= elapsedMs;
  
  Serial.printf("Entering deep sleep [ %lus ] ...\n", sleepMs/1000);
  ESP.deepSleep(sleepMs*1000);
}

void acquireAndSendData() {
  int vcc = ESP.getVcc();
  Serial.printf("VCC = %d mV\n", vcc);

  Serial.print("Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  Serial.println("DONE");
  // if we had multiple sensors on the same wire would could index them to get each reading
  // this project only has one DS18S20 sensor, so just take the first (and only) one
  float tempC = sensors.getTempCByIndex(0);

  // Arduino lib does not support printf'ing floats (%f does not work)
  // so let's do the formatting ourselves
  char strTempC[6];
  dtostrf(tempC, 3, 1, strTempC);
  Serial.printf("TEMP = %s C\n", strTempC);

  // Last but not least: connect to wifi and send data to Ubidots
  Serial.println("Connecting and sending...");
  client.wifiConnection(SSID, PASS);
  client.add(ID_METER1_BAT_VOLTAGE, vcc);
  client.add(ID_METER1_TEMPERATURE, tempC);
  client.sendAll();

  // for good measure
  delay(10);
}

void loop() {
  // after deep sleep we'll restart the boot sequence (as power on or reset) 
  // this means that setup() will be called over and over again and there's no
  // need or use in defining anything here in loop()
}

