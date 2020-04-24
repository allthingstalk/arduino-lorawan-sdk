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
 * In this example we present LoraOptions, which can be used to handle LoRa
 * specific metadata such as the uplink port, or message submission
 * acknowledgment mode. Changing modem specific settings such as the spreading
 * factor is presented as well.
 *
 * Before running the example, make sure that the LoRaWAN device is set up
 * on Maker with appropriate keys, and that a string asset named "message"
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
  // We initialize the serial connection so that we can show debug information.
  debugSerial.begin(57600);
  while ((!debugSerial)) {}

  // We initialize the modem.
  if (!modem.init()) {
    debugSerial.println("Could not initialize the modem. Check your keys.");
    exit(0);
  }

  // We set the lora options to port 4 with no acknowledgment.
  LoRaOptions options(4, false);
  modem.setOptions(options);
  // And we set the spreading factor to 7.
  modem.setSpreadingFactor(7);

  // Finally, we send the message.
  payload.set("message", "Hello LoRaOptions!");
  modem.send(payload);
}

void loop() {
  // Put additional code here, to run repeatedly.
}
