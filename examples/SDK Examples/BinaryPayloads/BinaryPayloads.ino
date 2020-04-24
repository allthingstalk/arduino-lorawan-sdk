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
 * In this example we implement a device that keeps its bandwidth usage low
 * by using custom binary payloads. The decoding logic for the binary data is
 * provided in the "Binary ABCL decoding.txt" file of this example.
 *
 * The provided binary format neatly packs three values: a single precision
 * floating point number, a boolean value, and a string that's five characters
 * long.

 * For more details on how to work with custom binary files please refer to
 * docs.allthingstalk.com and search for Binary Data Please note you will have
 * to create a new device for handling binary data as the decoding logic is
 * applicable at the device level & not on an asset level. To enter the decoding
 * logic, please navigate to Settings > Payload formats. Binary conversion logic
 * in this example also has the details of the asset names that need to be
 * configured for your device to visualise the data - "random", "toggle" and
 * "message". You are going to need a Sodaq ONE or Sodaq Mbili board. No sensors
 * required.
 *
 * Before running the example, make sure that the LoRaWAN device is set up
 * on Maker with appropriate keys, and that all assets are present on the device.
 */

#include <AllThingsTalk_LoRaWAN.h>
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
BinaryPayload payload;
bool toggle = false;

void setup() {
  debugSerial.begin(57600);
  while ((!debugSerial)) {}

  if (!modem.init()) {
    exit(0);
  }
}

void loop() {
  // In each iteration, we toggle the state of "toggle" between true and false.
  toggle = !toggle;

  // We reset the payload, and in order, add a random number, the current value
  // of "toggle", and a 5 character message.
  payload.reset();
  payload.add(randomNumber(0, 25));
  payload.add(toggle);
  payload.add("hello");

  // Then we send the payload.
  modem.send(payload);

  // Finally, we wait 10 seconds after each iteration,
  // so as not to spam the network.
  delay(10000);
}

float randomNumber(double minf, double maxf) {
  // Here, we generate a randomNumber and return it.
  return (minf + random(1UL << (sizeof(float) * 8 - 1)) * (maxf - minf) / (1UL << (sizeof(float) * 8 - 1)));
}
