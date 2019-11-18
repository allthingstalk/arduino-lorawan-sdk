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
 * In this example we implement a virtual GPS device which will publish random
 * GPS data to the asset created on the AllThingsTalk cloud. You are going to
 * need a Sodaq ONE or Sodaq Mbili board. No sensors required.
 *
 * Before running the example, make sure that the LoRaWAN device is set up
 * on Maker with appropriate keys, and that a location asset named "loc"
 * is present on the device.
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
CborPayload payload;

void setup() {
  debugSerial.begin(57600);
  while (!debugSerial) {}

  if (!modem.init()) {
    debugSerial.println("Could not initialize the modem. Check your keys.");
    exit(0);
  }
}

void loop() {
  // In each iteration, we generate random latitude, longitude and altitude
  // to use as our location.
  float latitude = randomFloat(-90, 90);
  float longitude = randomFloat(-180, 180);
  float altitude = randomFloat(0, 1000);
  GeoLocation geoLocation(latitude, longitude, altitude);

  // We reset the payload, and set "loc" to generated location data.
  payload.reset();
  payload.set("loc", geoLocation);

  // Then, we send the payload and increment the counter.
  modem.send(payload);

  // Finally, we wait 30 seconds after each iteration,
  // so as not to spam the network.
  delay(30000);
}

float randomFloat(float minf, float maxf) {
  return minf + random(1UL << 15) * (maxf - minf) / (1UL << 15);
}
