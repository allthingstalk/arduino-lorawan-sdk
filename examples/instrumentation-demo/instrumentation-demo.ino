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

MicrochipLoRaModem Modem(&Serial1, &SerialUSB);

void setup() 
{
  // Enable battery powered mode
  pinMode(ENABLE_PIN_IO, OUTPUT);
  digitalWrite(ENABLE_PIN_IO, HIGH);
  
  SerialUSB.begin(SERIAL_BAUD);                // set baud rate of the default serial debug connection
  while((!SerialUSB) && (millis()) < 10000){}  // wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run

  Serial1.begin(Modem.getDefaultBaudRate());   // init the baud rate of the serial connection so that it's ok for the modem
  while((!SerialUSB) && (millis()) < 30000){}  // wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run

  delay(3000);
  printInstrumentation();
  
  SerialUSB.println();
  SerialUSB.println("-- Program end --");
}

void printInstrumentation()
{
  SerialUSB.println("SYSTEM Parameters: ");
  SerialUSB.println("----------------- ");
  SerialUSB.print("Modem version: "); SerialUSB.println(Modem.getSysParam("ver"));
  SerialUSB.println();
  SerialUSB.println("MAC Parameters: ");
  SerialUSB.println("--------------- ");
  SerialUSB.print("DEVEUI: "); SerialUSB.println(Modem.getMacParam("deveui"));
  SerialUSB.print("DEVADDR: "); SerialUSB.println(Modem.getMacParam("devaddr"));
  SerialUSB.print("APPEUI: "); SerialUSB.println(Modem.getMacParam("appeuieui"));
  SerialUSB.print("Freq Band (Mhz): "); SerialUSB.println(Modem.getMacParam("band"));
  SerialUSB.print("Adaptive data rate: "); SerialUSB.println(Modem.getMacParam("adr"));
  SerialUSB.print("Data rate for nex transmission: "); SerialUSB.println(Modem.getMacParam("dr"));
  SerialUSB.print("Status: "); SerialUSB.println(Modem.getMacParam("status"));
  
  SerialUSB.println();
  SerialUSB.println("RADIO Parameters: ");
  SerialUSB.println("----------------- ");
  
  SerialUSB.print("Spreading factor: "); SerialUSB.println(Modem.getRadioParam("sf"));
  SerialUSB.print("Modulation Mode: "); SerialUSB.println(Modem.getRadioParam("mod"));
  SerialUSB.print("Operating Frequency: "); SerialUSB.println(Modem.getRadioParam("freq"));
}
