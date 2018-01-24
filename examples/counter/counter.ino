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
 
// Uncomment your selected method for sending data
//#define CONTAINERS
#define CBOR
//#define BINARY

// Select your hardware
#define SODAQ_MBILI
//#define SODAQ_ONE

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

#ifdef CONTAINERS
  #include <Container.h>
  Container container(device);
#endif

#ifdef CBOR
  #include <CborBuilder.h>
  CborBuilder payload(device);
#endif

#ifdef BINARY
  #include <PayloadBuilder.h>
  PayloadBuilder payload(device);
#endif

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
  #ifdef CONTAINERS
  container.addToQueue(counter, INTEGER_SENSOR, false);  
  #endif
  
  #ifdef CBOR
  payload.reset();
  payload.map(1);
  payload.addInteger(counter, "15");
  payload.addToQueue(false);
  #endif

  #ifdef BINARY  
  payload.reset();
  payload.addInteger(counter);
  payload.addToQueue(false);
  #endif
  
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
    sendNextAt = millis() + 30000;
  }
}