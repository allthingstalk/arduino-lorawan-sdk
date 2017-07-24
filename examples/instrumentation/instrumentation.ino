*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
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

#include <MicrochipLoRaModem.h>

#define SERIAL_BAUD 57600

// Sodaq ONE
#define debugSerial SerialUSB
#define loraSerial Serial1

// Sodaq Mbili
//#define debugSerial Serial
//#define loraSerial Serial1

MicrochipLoRaModem modem(&loraSerial, &debugSerial);

void setup() 
{
  debugSerial.begin(SERIAL_BAUD);                // set baud rate of the default serial debug connection
  while((!debugSerial) && (millis()) < 10000){}  // wait until serial bus is available, so we get the correct logging on screen. If no serial, then blocks for 2 seconds before run

  Serial1.begin(modem.getDefaultBaudRate());   // init the baud rate of the serial connection so that it's ok for the modem
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
  debugSerial.print("modem version: "); debugSerial.println(mdem.getSysParam("ver"));
  debugSerial.println();
  debugSerial.println("MAC Parameters: ");
  debugSerial.println("--------------- ");
  debugSerial.print("DEVEUI: "); debugSerial.println(modem.getMacParam("deveui"));
  debugSerial.print("DEVADDR: "); debugSerial.println(modem.getMacParam("devaddr"));
  debugSerial.print("APPEUI: "); debugSerial.println(modem.getMacParam("appeuieui"));
  debugSerial.print("Freq Band (Mhz): "); debugSerial.println(modem.getMacParam("band"));
  debugSerial.print("Adaptive data rate: "); debugSerial.println(modem.getMacParam("adr"));
  debugSerial.print("Data rate for nex transmission: "); debugSerial.println(modem.getMacParam("dr"));
  debugSerial.print("Status: "); debugSerial.println(modem.getMacParam("status"));
  
  debugSerial.println();
  debugSerial.println("RADIO Parameters: ");
  debugSerial.println("----------------- ");
  
  debugSerial.print("Spreading factor: "); debugSerial.println(modem.getRadioParam("sf"));
  debugSerial.print("Modulation Mode: "); debugSerial.println(modem.getRadioParam("mod"));
  debugSerial.print("Operating Frequency: "); debugSerial.println(modem.getRadioParam("freq"));
}

void loop()
{
}
