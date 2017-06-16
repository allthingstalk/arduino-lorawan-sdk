/*
   Copyright 2015-2016 AllThingsTalk

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
 * 
 *  THIS SKETCH IS FOR THE SODAQ ONE BOARD
 *  
 *  
 *  For more information, please check our documentation
 *  -> http://allthingstalk.com/docs/tutorials/lora/setup
 *  
 **/

//#include <Wire.h>
#include <ATT_IOT_LoRaWAN.h>
#include "keys.h"
#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600

MicrochipLoRaModem Modem(&Serial1, &SerialUSB);
ATTDevice Device(&Modem, &SerialUSB);

struct DemoData
{
	short value1;
	unsigned char value2;
	unsigned char value3;
		
};


DemoData data;

void setup() 
{
  digitalWrite(ENABLE_PIN_IO, HIGH);
  delay(3000);
  
  SerialUSB.begin(SERIAL_BAUD);
  Serial1.begin(Modem.getDefaultBaudRate());					// init the baud rate of the serial connection so that it's ok for the modem
  while((!SerialUSB) && (millis()) < 30000){}            //wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run
  
  while(!Device.InitABP(DEV_ADDR, APPSKEY, NWKSKEY));
  SerialUSB.println("Ready to send data");
}

int counter = 0;
unsigned long sendNextAt = 0;
int sendState = 1;

void loop() 
{
	data.value1 = counter;
	data.value2 = counter;
	data.value3 = counter;
	if(sendNextAt < millis()){
		bool sendSuccess = Device.AddToQueue(&data, sizeof(data));  // non blocking
    if(sendSuccess == false){
      SerialUSB.println("discarding data");
    }
		counter++;
		sendNextAt = millis() + 10000;
	}
	sendState = Device.ProcessQueue();
	if(sendState == -1){
    SerialUSB.println("failed to send data, removing from queue");
    Device.Pop();
  }
}



