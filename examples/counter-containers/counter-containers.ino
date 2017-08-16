/*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
 *   /_\ | | |_   _| |_ (_)_ _  __ _ __|_   _|_ _| | |__ / __|   \| |/ /
 *  / _ \| | | | | | ' \| | ' \/ _` (_-< | |/ _` | | / / \__ \ |) | ' <
 * /_/ \_\_|_| |_| |_||_|_|_||_\__, /__/ |_|\__,_|_|_\_\ |___/___/|_|\_\
 *                             |___/
 *
 * Copyright 2017 AllThingsTalk
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

#include <ATT_IOT_LoRaWAN.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>
#include <Container.h>

#define SERIAL_BAUD 57600

// Sodaq ONE
//#define debugSerial SerialUSB
//#define loraSerial Serial1

// Sodaq Mbili
#define debugSerial Serial
#define loraSerial Serial1

MicrochipLoRaModem modem(&loraSerial, &debugSerial);
ATTDevice device(&modem, &debugSerial, false, 7000);  // minimum time between 2 messages set at 7000 milliseconds
Container container(device);

void setup() 
{
  delay(3000);
  
  debugSerial.begin(SERIAL_BAUD);
  loraSerial.begin(modem.getDefaultBaudRate());  // init the baud rate of the serial connection so that it's ok for the modem
  while((!debugSerial) && (millis()) < 30000){}
  
  while(!device.initABP(DEV_ADDR, APPSKEY, NWKSKEY));
  debugSerial.println("Ready to send data");
}

void sendValue(int16_t counter)
{
  container.addToQueue(counter, INTEGER_SENSOR, false);
  
  device.processQueue();
}

int16_t counter = 0;
unsigned long sendNextAt = 0;
void loop() 
{
  if(sendNextAt < millis())
  {
    sendValue(counter);
    counter++;
    sendNextAt = millis() + 8000;
  }
}