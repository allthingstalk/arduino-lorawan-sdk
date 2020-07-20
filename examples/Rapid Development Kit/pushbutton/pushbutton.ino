/*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
 *   /_\ | | |_   _| |_ (_)_ _  __ _ __|_   _|_ _| | |__ / __|   \| |/ /
 *  / _ \| | | | | | ' \| | ' \/ _` (_-< | |/ _` | | / / \__ \ |) | ' <
 * /_/ \_\_|_| |_| |_||_|_|_||_\__, /__/ |_|\__,_|_|_\_\ |___/___/|_|\_\
 *                             |___/
 *
 * Copyright 2020 AllThingsTalk
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
 * ------- About This Example -------
 * If you didn't read the guide on AllThingsTalk Knowledge Center,
 * please do so at: https://www.allthingstalk.com/faq/getting-started-with-the-lorawan-rapid-development-kit
 * 
 * When the button is pressed, a value "true" is sent to AllThingsTalk.
 * The asset name for the button is "3", which is the "Push Button" asset.
 */

#include <AllThingsTalk_LoRaWAN.h>                      // Load the AllThingsTalk LoRaWAN SDK
#include "keys.h"                                       // Load the header file that contains your credentials for LoRaWAN
#define debugSerial Serial                              // Define the serial interface that's going to be used for Serial monitor (debugging)
#define loraSerial Serial1                              // Define the serial interface that'll be used for communication with the LoRa module

ABPCredentials credentials(DEVADDR, APPSKEY, NWKSKEY);  // Define the credential variables loaded from the keys.h file
LoRaModem modem(loraSerial, debugSerial, credentials);  // Define LoRa modem properties
CborPayload payload;

int   buttonPin           = 4;                          // Pin number of the button
bool  prevButtonState     = false;                      // Initial state of the previous button state

void setup() {
  debugSerial.begin(57600);                             // Starts the serial output 
  pinMode(buttonPin, INPUT);                            // Initialize the button pin on the board
//  while ((!debugSerial)) {}                             // (Uncomment if you need this. Keep in mind that the device won't work if you don't open the Serial monitor) Halts the whole device until the Serial monitor is opened on your computer so you can see all Serial messages from the board
  if (!modem.init()) {                                  // Writes the message below if the modem can't be initialized
    debugSerial.println("Could not initialize the modem. Check your keys.");
    exit(0);                                            // And exits
  } else {                                              // Otherwise prints the text below to Serial
    debugSerial.println("Model initialized succesfully!");
  }
}

void pushButton() {                                     // Name of our function that we'll call from the loop function below
  bool buttonState = digitalRead(buttonPin);            // Read the status of the button and save it in 'buttonState'
  if (buttonState == true) {                            // If the button is pressed
    if (prevButtonState == false) {                     // If the button previously wasn't pressed. This way the code below won't be run multiple times if you press and keep pressing the button.
      debugSerial.println("Button press detected, now sending...");
      payload.reset();                                  // Resets the payload in case there's anything left
      payload.set("3", buttonState);                    // Sets the payload with our entity name "3" and the value of our button (true/1)
      modem.send(payload);                              // Sends the payload via LoRaWAN
      prevButtonState = true;                           // Sets the previous button state to pressed (so the 'if' statement above fails) 
    }
  } else { 
    prevButtonState = false;                            // If the button is not being pressed, set the value of previous button state to False
  }
}

void loop() {
  pushButton();                                         // Runs our 'pushButton' function above in a loop
}
