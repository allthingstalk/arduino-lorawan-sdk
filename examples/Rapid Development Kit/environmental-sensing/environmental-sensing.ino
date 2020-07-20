/*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
 *   /_\ | | |_   _| |_ (_)_ _  __ _ __|_   _|_ _| | |__ / __|   \| |/ /
 *  / _ \| | | | | | ' \| | ' \/ _` (_-< | |/ _` | | / / \__ \ |) | ' <
 * /_/ \_\_|_| |_| |_||_|_|_||_\__, /__/ |_|\__,_|_|_\_\ |___/___/|_|\_\
 *                             |___/
 *
 * Copyright 2020 AllThingsTalk
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 * 
 * ------- About This Example -------
 * If you didn't read the guide on AllThingsTalk Knowledge Center,
 * please do so at: https://www.allthingstalk.com/faq/getting-started-with-the-lorawan-rapid-development-kit
 * 
 * This experiment shows how LoRa can be used to monitor the quality of
 * your surrounding environment. Measure in and outdoor air quality, noise
 * levels and temperature to provide stakeholders a dashboard to support
 * their decision making to improve quality of living.
 */

#include <AllThingsTalk_LoRaWAN.h>                      // Load the AllThingsTalk LoRaWAN SDK
#include "keys.h"                                       // Load the header file that contains your credentials for LoRaWAN
#include <Wire.h>                                       // Library used for I2C communication
#include "Adafruit_BME280.h"                            // Adafruit's library for the BME280 Humidity, Temperature and Pressure sensor
#include "Seeed_BME280.h"                               // Seed library for the BME280 Humidity, Temperature and Pressure sensor

#define AirQualityPin   A0                              // Pin number to which our Air Quality (gas) sensor is connected to
#define LightSensorPin  A2                              // Pin number to which our Light Sensor is connected to
#define SoundSensorPin  A4                              // Pin number to which our Sound Sensor is connected to
#define debugSerial     Serial                          // Define the serial interface that's going to be used for Serial monitor (debugging)
#define loraSerial      Serial1                         // Define the serial interface that'll be used for communication with the LoRa module
#define debugSerialBaud 57600                           // Define the baud rate for the debugging serial port (used for Serial monitor)

float soundValue, lightValue, temperature, humidity, pressure, airValue;    // Variables used to store our sensor data
unsigned int sendEvery = 300;                           // Creates a delay so the data is not constantly sent. 
uint8_t sensorType;                                     // Determines tph sensor type

ABPCredentials credentials(DEVADDR, APPSKEY, NWKSKEY);  // Define the credential variables loaded from the keys.h file
LoRaModem modem(loraSerial, debugSerial, credentials);  // Define LoRa modem properties
Adafruit_BME280 tph1;                                   // Create an "tph" object which will be our TPH (Temperature, Pressure and Humidity) sensor
BME280 tph2;
CborPayload payload;                                    // Create an object for our cbor payload which will be used to send data to AllThingsTalk

void setup() {                                          // This function runs only at boot and only once.
  debugSerial.begin(debugSerialBaud);                   // Initialize the debug serial port (for Serial monitor)
  while((!debugSerial) && (millis()) < 10000){}         // Wait for the Serial monitor to be open (so you can see all output) and if it isn't open in 10 seconds, run the sketch
  while (!modem.init()) { delay(1000); }                // Initialize the modem. If it fails, retry every 1 second until it succeeds
  pinMode(GROVEPWR, OUTPUT);                            // Set the secondary row of grove connectors to OUTPUT (so we can turn it on)
  digitalWrite(GROVEPWR, HIGH);                         // Turn on the power for the secondary row of grove connectors (the switched row on the board) 
  pinMode(SoundSensorPin, INPUT);                       // Initialize the Sound Sensor pin as INPUT, so we can read the data
  pinMode(AirQualityPin, INPUT);                        // Initialize the Air Quality (Gas) Sensor pin as INPUT so we can read the data
  pinMode(LightSensorPin, INPUT);                       // Initialize the Light Sensor pin as INPUT, so we can read the data
  initTphSensor();                                      // Begin the i2c connection for the BME280 sensor (the Temperature, Pressure and Humidity Sensor)
}

void initTphSensor() {
  if (tph1.begin()) {
    // Sensor will use Adafruit Library
    sensorType = 1;
  } else if (tph2.init()) {
    // Sensor will use Seeed Library
    sensorType = 2;
  } else {    
    debugSerial.println("Could not initialize TPH sensor, please check wiring.");
    sensorType = 0;
  }
}

void readSensors() {                                    // Function that we'll call when we want to read the data from all sensors
  soundValue  = analogRead(SoundSensorPin);             // Read the data from the Sound Sensor pin and save it into the "soundValue" variable
  airValue    = analogRead(AirQualityPin);              // Read the data from the Air Quality (gas) sensor and save it into the "airValue" variable
  lightValue  = analogRead(LightSensorPin);             // Read the data from the Light Sensor pin and save it into the "lightValue" variable
  lightValue  = lightValue * 3.3 / 1023;                // Convert the light value to lux based on the voltage that the sensor receives (3.3 volts)
  lightValue  = pow(10, lightValue);                    // Light value (converted to lux) raised to power of 10

  if (sensorType == 1) {
    temperature = tph1.readTemperature();               // Read the temperature data from the BME280 (TPH) Sensor and save it into the "temperature" variable
    humidity    = tph1.readHumidity();                  // Read the humidity data from the BME280 (TPH) Sensor and save it into the "humidity" variable
    pressure    = tph1.readPressure()/100.0;            // Read the pressure data from the BME280 (TPH) Sensor, divide it by 100 and save it into the "pressure" variable
  } else if (sensorType == 2) {
    temperature = tph2.getTemperature();                // Read the temperature data from the BME280 (TPH) Sensor and save it into the "temperature" variable
    humidity    = tph2.getHumidity();                   // Read the humidity data from the BME280 (TPH) Sensor and save it into the "humidity" variable
    pressure    = tph2.getPressure()/100.0;             // Read the pressure data from the BME280 (TPH) Sensor, divide it by 100 and save it into the "pressure" variable
  } else {
    temperature = 0;
    humidity = 0;
    pressure = 0;
    debugSerial.println("TPH Sensor seems to be disconnected. Temperature/Humidity/Pressure Values sent will be 0.");
  }
}

void sendSensorValues() {                               // Function used to send the data we collected from all the sensors
  payload.reset();                                      // Reset the cbor payload (in case there's anything left from the previous one)
  payload.set("12", soundValue);                        // Create a payload containing data from the Sound Sensor to be sent to asset name "12" (Loudness sensor asser on AllThingsTalk Maker)
  payload.set("6", lightValue);                         // Create a payload containing data from the Light Sensor to be sent to asset name "6" (Light sensor asset on AllThingsTalk Maker)
  payload.set("5", temperature);                        // Create a payload containing data from the Temperature Sensor (TPH) to be sent to asset name "5" (Temperature sensor asset on AllThingsTalk Maker)
  payload.set("11", humidity);                          // Create a payload containing data from the Humidity Sensor (TPH) to be sent to asset name "11" (Humidity sensor asset on AllThingsTalk Maker)
  payload.set("10", pressure);                          // Create a payload containing data from the Pressure Sensor (TPH) to be sent to asset name "10" (Pressure sensor asset on AllThingsTalk Maker)
  payload.set("13", airValue);                          // Create a payload containing data from the Air Quality (gas) Sensor to be sent to the asset name "13" (Air quality sensor asset on AllThingsTalk Maker)
  modem.send(payload);                                  // Send all the data above to your AllThingsTalk account
}

void displaySensorValues() {                            // Function we'll call to output all the collected data via debug Serial (to see when you open Serial Monitor)
  debugSerial.println("-----------------------");       // A simple line to distinguish between old and new data easily
  
  debugSerial.print("Sound level: ");                   // debugSerial.print outputs some text to serial but doesn't add a new line at the end
  debugSerial.print(soundValue);                        // Outputs value of variable "soundValue" to serial (also without adding a new line at the end)
  debugSerial.println(" Analog (0-1023)");              // Outputs text to serial but adds a new line at the end.

  debugSerial.print("Light intensity: ");
  debugSerial.print(lightValue);
  debugSerial.println(" Lux");

  debugSerial.print("Temperature: ");
  debugSerial.print(temperature);
  debugSerial.println(" °C");

  debugSerial.print("Humidity: ");
  debugSerial.print(humidity);
	debugSerial.println(" %");

  debugSerial.print("Pressure: ");
  debugSerial.print(pressure);
	debugSerial.println(" hPa");

  debugSerial.print("Air quality: ");
  debugSerial.print(airValue);
	debugSerial.println(" Analog (0-1023)");
  
  delay(200);                                           // A necessary delay so the serial output isn't cut off while outputting
}

void loop() {                                           // This function runs automatically in a continuous loop as long as the device is powered on
  readSensors();                                        // Calls the function that reads all the sensor data
  displaySensorValues();                                // Calls the function that displays all the sensor data via debug serial (to see in Serial monitor)
  sendSensorValues();                                   // Calls the function that sends all the sensor data to AllThingsTalk Maker via LoRa
  delay(sendEvery*1000);                                // Creates a delay so the data is not constantly sent. The delay is defined in the "sendEvery" variable at the beginning.
}
