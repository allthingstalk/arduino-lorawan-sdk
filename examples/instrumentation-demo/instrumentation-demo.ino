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
 *  Version 1.0 dd 16/06/2017
 *  Original author: Peter Leemans 2017
 *
 *  This example sketch shows how to retrieve instrumentation info
 **/

#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600

// Sodaq ONE
#define debugSerial SerialUSB
#define loraSerial Serial1

// Sodaq Mbili
//#define debugSerial Serial
//#define loraSerial Serial1

MicrochipLoRaModem Modem(&loraSerial, &debugSerial);

void setup() 
{
  debugSerial.begin(SERIAL_BAUD);                // set baud rate of the default serial debug connection
  while((!debugSerial) && (millis()) < 10000){}  // wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run

  Serial1.begin(Modem.getDefaultBaudRate());   // init the baud rate of the serial connection so that it's ok for the modem
  while((!debugSerial) && (millis()) < 30000){}  // wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run

  delay(3000);
  printInstrumentation();
  
  debugSerial.println();
  debugSerial.println("-- Program end --");
}

void printInstrumentation()
{
  debugSerial.println("SYSTEM Parameters: ");
  debugSerial.println("----------------- ");
  debugSerial.print("Modem version: "); debugSerial.println(Modem.getSysParam("ver"));
  debugSerial.println();
  debugSerial.println("MAC Parameters: ");
  debugSerial.println("--------------- ");
  debugSerial.print("DEVEUI: "); debugSerial.println(Modem.getMacParam("deveui"));
  debugSerial.print("DEVADDR: "); debugSerial.println(Modem.getMacParam("devaddr"));
  debugSerial.print("APPEUI: "); debugSerial.println(Modem.getMacParam("appeuieui"));
  debugSerial.print("Freq Band (Mhz): "); debugSerial.println(Modem.getMacParam("band"));
  debugSerial.print("Adaptive data rate: "); debugSerial.println(Modem.getMacParam("adr"));
  debugSerial.print("Data rate for nex transmission: "); debugSerial.println(Modem.getMacParam("dr"));
  debugSerial.print("Status: "); debugSerial.println(Modem.getMacParam("status"));
  
  debugSerial.println();
  debugSerial.println("RADIO Parameters: ");
  debugSerial.println("----------------- ");
  
  debugSerial.print("Spreading factor: "); debugSerial.println(Modem.getRadioParam("sf"));
  debugSerial.print("Modulation Mode: "); debugSerial.println(Modem.getRadioParam("mod"));
  debugSerial.print("Operating Frequency: "); debugSerial.println(Modem.getRadioParam("freq"));
}

void loop()
{
}
