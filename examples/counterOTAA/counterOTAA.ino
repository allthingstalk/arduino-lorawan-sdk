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
 
// Select your hardware
//#define SODAQ_MBILI
#define SODAQ_ONE

/***************************************************************************/

#include <ATT_LoRaWAN.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600

#ifdef SODAQ_ONE
  #define debugSerial SerialUSB
  #define loraSerial Serial1
#endif

#ifdef SODAQ_MBILI
  #define debugSerial Serial
  #define loraSerial Serial1
#endif

MicrochipLoRaModem modem(&loraSerial, &debugSerial);
ATTDevice device(&modem, &debugSerial, false, 7000);  // minimum time between 2 messages set at 7000 milliseconds

#include <CborBuilder.h>
CborBuilder payload(device);

void setup() 
{
  debugSerial.begin(SERIAL_BAUD);
  while((!debugSerial) && (millis()) < 10000){}  // wait until the serial bus is available
  
  loraSerial.begin(modem.getDefaultBaudRate());  // set baud rate of the serial connection to match the modem
  while((!loraSerial) && (millis()) < 10000){}   // wait until the serial bus is available
  
  while(!device.initOTAA(DEV_EUI, APPS_EUI, APP_KEY, false));
  debugSerial.println("Ready to send data");
}

void process()
{
  while(device.processQueue() > 0)
  {
    debugSerial.print("QueueCount: ");
    debugSerial.println(device.queueCount());
    delay(10000);
  }
}

void sendValue(int16_t counter)
{
  payload.reset();
  payload.map(1);
  payload.addInteger(counter, "15");
  payload.addToQueue(false);
  process();
}

short counter = 0;
unsigned long sendNextAt = 0;
void loop() 
{
  if(sendNextAt < millis())
  {
    sendValue(counter);
    counter++;
    sendNextAt = millis() + 30000;
  }
}
