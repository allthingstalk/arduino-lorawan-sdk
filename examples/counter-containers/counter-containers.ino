/*
  Copyright 2015-2017 AllThingsTalk

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

/****
 *  AllThingsTalk Developer Cloud IoT experiment for LoRa
 *  Version 1.0 dd 09/11/2015
 *  Original author: Jan Bogaerts 2015
 *
 *  This sketch is part of the AllThingsTalk LoRa rapid development kit
 *  -> http://www.allthingstalk.com/lora-rapid-development-kit
 *
 *  This example sketch is based on the Proxilmus IoT network in Belgium
 *  The sketch and libs included support the
 *  - MicroChip RN2483 LoRa module
 **/

#include <ATT_IOT_LoRaWAN.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>
#include <Container.h>

#define SERIAL_BAUD 57600

// Sodaq ONE
#define debugSerial SerialUSB
#define loraSerial Serial1

// Sodaq Mbili
//#define debugSerial Serial
//#define loraSerial Serial1

MicrochipLoRaModem Modem(&loraSerial, &debugSerial);
ATTDevice Device(&Modem, &debugSerial);
Container container(Device);

void setup() 
{
  //digitalWrite(ENABLE_PIN_IO, HIGH);
  delay(3000);
  
  debugSerial.begin(SERIAL_BAUD);
  loraSerial.begin(Modem.getDefaultBaudRate());  // init the baud rate of the serial connection so that it's ok for the modem
  while((!debugSerial) && (millis()) < 30000){}  // wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run
  
  while(!Device.InitABP(DEV_ADDR, APPSKEY, NWKSKEY));
  debugSerial.println("Ready to send data");
}

int sendState = 1;

void sendValue(int counter)
{
  bool res = container.AddToQueue(counter, INTEGER_SENSOR);
  
  sendState = Device.ProcessQueue();
	if(sendState == -1)
  {
    debugSerial.println("Failed to send data; removing from queue");
    Device.Pop();
  }
}

int counter = 0;
unsigned long sendNextAt = 0;
void loop() 
{
  if(sendNextAt < millis())
  {
    sendValue(counter);
    counter++;
		sendNextAt = millis() + 900000;  // send every 15 minutes
	}
}