/*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
 *   /_\ | | |_   _| |_ (_)_ _  __ _ __|_   _|_ _| | |__ / __|   \| |/ /
 *  / _ \| | | | | | ' \| | ' \/ _` (_-< | |/ _` | | / / \__ \ |) | ' <
 * /_/ \_\_|_| |_| |_||_|_|_||_\__, /__/ |_|\__,_|_|_\_\ |___/___/|_|\_\
 *                             |___/
 *
 * Copyright 2018 AllThingsTalk
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
 */
 
/*
 * This basic example gives you a good overview of an example sketch. It
 * has a simple push button that acts as a toggle and sends the state true
 * or false to the AllThingsTalk cloud.
 */
/***************************************************************************/



#include <ATT_LoRaWAN.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600

// Select your hardware
#define SODAQ_MBILI
//#define SODAQ_ONE

#ifdef SODAQ_ONE
  #define debugSerial SerialUSB
  #define loraSerial Serial1
#endif

#ifdef SODAQ_MBILI
  #define debugSerial Serial
  #define loraSerial Serial1
#endif


void callback(const unsigned char* payload, unsigned int length, uint8_t port );
MicrochipLoRaModem modem(&loraSerial, &debugSerial, callback);
ATTDevice device(&modem, &debugSerial, false, 7000);  // Minimum time between 2 messages set at 7000 milliseconds


#include <CborBuilder.h>
CborBuilder payload(device);


int digitalSensor = 20;  // Digital sensor is connected to pin D20/21

void process()
{
  while(device.processQueue(1) > 0)
  {
    debugSerial.print("QueueCount: ");
    debugSerial.println(device.queueCount());
    delay(10000);
  }
}

void sendValue(bool val)
{
  payload.reset();
  payload.map(1);
  payload.addBoolean(val, "b");
  payload.addToQueue(false);
  process();
}

void setup() 
{  
  pinMode(digitalSensor, INPUT);  // Initialize the digital pin as an input
  delay(3000);
  
  debugSerial.begin(SERIAL_BAUD);
  while((!debugSerial) && (millis()) < 10000){}  // Wait until the serial bus is available
  
  loraSerial.begin(modem.getDefaultBaudRate());  // Set baud rate of the serial connection to match the modem
  while((!loraSerial) && (millis()) < 10000){}   // Wait until the serial bus is available

  while(!device.initABP(DEV_ADDR, APPSKEY, NWKSKEY))
  debugSerial.println("Ready to send data");

  // send initial value
  sendValue(false);
}

bool sensorVal = false;
bool prevButtonState = false;

void loop() 
{
  bool sensorRead = digitalRead(digitalSensor);     // Read status Digital Sensor
  if (sensorRead == 1 && prevButtonState == false)  // Verify if value has changed
  {
    prevButtonState = true;
    debugSerial.println("Button pressed");
    sendValue(!sensorVal);
    sensorVal = !sensorVal;
  }
  else if(sensorRead == 0)
    prevButtonState = false;
}

// callback function
// handle messages that were sent from the AllThingsTalk cloud to this device
void callback(const unsigned char* payload, unsigned int length, uint8_t port ) 
{
   String msgString;
  
   char message_buff[length + 1];
   strncpy(message_buff, (char*)payload, length);
   message_buff[length] = '\0';
   msgString = String(message_buff);

   Serial.print("Payload: ");
   Serial.println(msgString);
   
}
