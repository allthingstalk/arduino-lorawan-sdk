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
 * In this example, we show how to process downlinks. You are going to need a
 * Sodaq ONE or Sodaq Mbili board. No sensors required.
 *
 * Before running the example, make sure that the LoRaWAN device is set up
 * on Maker with appropriate keys. Choose a LoRa network with actuation support,
 * like TheThingsNetwork. Finally, create a sensor string asset "greeting", and
 * an actuator named "actuator".
 *
 * To see how downlinks work, send commands to "actuator" using Maker, while the
 * device is running. Received data will be visibile in your Serial monitor.
 */

#include <ABPCredentials.h>
#include <LoRaModem.h>
#include <LoRaOptions.h>
#include <CborPayload.h>
#include <BinaryPayload.h>

#include "keys.h"

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

ABPCredentials credentials(DEVADDR, APPSKEY, NWKSKEY);
LoRaModem modem(loraSerial, debugSerial, credentials);
CborPayload payload;

void setup() {
  debugSerial.begin(57600);
  while (!debugSerial) {}

  if (!modem.init()) {
    debugSerial.println("Could not initialize the modem. Check your keys.");
    exit(0);
  }

  // Because we're going to process downlinks, we set a downlink callback
  // to the the processing there once the downlink occurs.
  modem.setDownlinkCallback(callback);
}

void loop() {
  // In order to receive data using LoRa, we need to send some data first.
  // Every iteration, we select a different name from the list below
  // and send it to the platform.
  String names[] = {"Lora", "Sky", "Trinity", "Xena", "Avalon", "Nyo", "Vixen"};
  payload.reset();
  payload.set("greeting", String("Hello " + names[random(7)]));
  modem.send(payload);

  // Finally, we wait 10 seconds after each iteration,
  // so as not to spam the network.
  delay(10000);
}


void callback(BinaryPayload &payload, LoRaOptions &options) {
  // When processing downlinks, we just print out the debug
  // information together with the received data.
  unsigned char *bytes = payload.getBytes();
  debugSerial.println("In callback function!");
  for (auto i = 0; i < payload.getSize(); i++) {
    debugSerial.print(bytes[i], HEX);
  }
  debugSerial.println();
}
