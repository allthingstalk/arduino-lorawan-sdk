/*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
 *   /_\ | | |_   _| |_ (_)_ _  __ _ __|_   _|_ _| | |__ / __|   \| |/ /
 *  / _ \| | | | | | ' \| | ' \/ _` (_-< | |/ _` | | / / \__ \ |) | ' <
 * /_/ \_\_|_| |_| |_||_|_|_||_\__, /__/ |_|\__,_|_|_\_\ |___/___/|_|\_\
 *                             |___/
 *
 * Copyright 2019 AllThingsTalk
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
 * ----------------------------------------------------------------------------
 *
 * Before you start please ensure that both the library and the board is
 * updated to the latest version.
 *
 ******************************************************************************
 * About this example
 ******************************************************************************
 *
 * In this example, we don't send any data to the AllThingsTalk Cloud. Instead,
 * we use the SDK capabilities to print out the data we can retrieve from the
 * LoRa modem.
 */

#include <AllThingsTalk_LoRaWAN.h>

// This example supports both Sodaq ONE & Sodaq Mbili.
// The code below helps us discover which one it is.

#if defined(ARDUINO_SODAQ_ONE)
  #define debugSerial SerialUSB
#elif defined(ARDUINO_AVR_SODAQ_MBILI)
  #define debugSerial Serial
#else
  #error "Unsupported board."
#endif
#define loraSerial Serial1

LoRaModem modem(loraSerial, debugSerial);

void setup() {
  // We initialize the serial connection so that we can show debug information.
  debugSerial.begin(57600);
  while (!debugSerial) {}

  // We initialize the modem and print out all the information we can get from
  // it using the serial connection.
  if (modem.init()) {
    debugSerial.println("------------------------------------------------------------");
    debugSerial.print("Modem version: ");
    debugSerial.println(modem.getModemVersion());
    debugSerial.print("Frequency band: ");
    debugSerial.println(modem.getFrequencyBand());
    debugSerial.print("Adaptive data rate: ");
    debugSerial.println(modem.getAdaptiveDataRate());
    debugSerial.print("Data rate: ");
    debugSerial.println(modem.getDataRate());
    debugSerial.print("Modem status: ");
    debugSerial.println(modem.getStatus());
    debugSerial.print("Spreading factor: ");
    debugSerial.println(modem.getSpreadingFactor());
    debugSerial.print("Modulation mode: ");
    debugSerial.println(modem.getModulationMode());
    debugSerial.print("Operation frequency: ");
    debugSerial.println(modem.getOperationFrequency());
  }
}

void loop() {
  // Put additional code here, to run repeatedly.
}
