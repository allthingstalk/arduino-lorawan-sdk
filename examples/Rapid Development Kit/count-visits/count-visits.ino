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
 * Uses a reed switch to check when the door opens, waits for the door to close and then increments the 
 * number of visits and uploads that data to AllThingsTalk. Also uses a momentary push button which, 
 * when clicked, resets the visits counter to zero and uploads that data as well.
 * The asset name for the reed switch/door sensor is "15", which is the "Integer sensor" asset.
 */

#include <AllThingsTalk_LoRaWAN.h>
#include "keys.h"                                       // Load the header file that contains your credentials for LoRaWAN
#define debugSerial Serial                              // Define the serial interface that's going to be used for Serial monitor (debugging)
#define loraSerial Serial1                              // Define the serial interface that'll be used for communication with the LoRa module

ABPCredentials credentials(DEVADDR, APPSKEY, NWKSKEY);  // Define the credential variables loaded from the keys.h file (for ABP activation method)
LoRaModem modem(loraSerial, debugSerial, credentials);  // Define LoRa modem properties
CborPayload payload;

int   reedPin         = 20;                             // Pin number to which the door sensor (reed switch) is connected
int   buttonPin       = 4;                              // Pin number to which the button is connected

int   visitCount      = 0;                              // Initial value for visit count
bool  prevButtonState = false;                          // Variable used to store previous button state, used to track changes of the button
bool  prevDoorState   = false;                          // Variable used to store previous door state, used to track changes of door state
bool  waitToClose     = false;                          // Variable used for the program to know if it's supposed to wait for the door to close

void setup() {                                          // The setup function is only called once, on boot.
  debugSerial.begin(57600);                             // Starts the serial output at baud rate of 57600 
  pinMode(reedPin, INPUT_PULLUP);                       // Initialize the reed (door sensor) pin on the board as input and enable it's internal pullup resistor
  pinMode(buttonPin, INPUT);                            // Initialize the button pin on the board as input
//  while ((!debugSerial)) {}                             // (Uncomment if you need this. Keep in mind that the device won't work if you don't open the Serial monitor) Halts the whole device until the Serial monitor is opened on your computer so you can see all Serial messages from the board
  if (!modem.init()) {                                  // Tries to initialize the LoRa modem, and writes the message below if the modem can't be initialized
    debugSerial.println("Could not initialize the modem. Check your keys.");
    exit(0);
  } else {
    debugSerial.println("Model initialized succesfully!"); // Prints this to serial only if the modem is successfully initialized
  }
}

void pushButton() {                                     // Function that checks/does everything regarding the pushbutton
  bool buttonState = digitalRead(buttonPin);            // Read the status of the button and save it in 'buttonState'
  // Here we're going to track the state change of the button. If we didn't do this, the program would run the code below in an endless loop
  // as long as the button stays pressed. The variable "prevButtonState" is used so the program is aware of the button state before the change occured.
  // This way, when the button is pressed once, the block of code below is executed only once as well.
  if (buttonState == true) {                            // If the button is pressed
    if (prevButtonState == false) {                     // If the button previously wasn't pressed. This way the code below won't be run multiple times if you press and keep pressing the button.
      visitCount = 0;                                   // Reset the value of 'visitCount', which is the variable we use to store how many times the door was opened+closed
      debugSerial.print("Button pressed. Visits count reset to ");
      debugSerial.println(visitCount);                  // Writes to Serial so that you can see what's going on
      payload.reset();                                  // Resets the payload in case there's anything left
      payload.set("15", visitCount);                    // Sets the payload "visitCount" with our entity name "15" (Integer Sensor)
      modem.send(payload);                              // Sends the payload via LoRaWAN
      prevButtonState = true;                           // Sets the previous button state to pressed (so the 'if' statement above fails) 
    }
  } else { 
    prevButtonState = false;                            // If the button is not being pressed, set the value of previous button state to False
  }
}

void countVisits() {                                    // Name of our function that we'll call from the loop function below
  bool doorState = digitalRead(reedPin);                // Read the status of our reed switch (door sensor pin) and save it in 'doorState' variable.
  if (doorState == true && waitToClose == false) {      // Runs the two lines below only if the door is open AND if it's not waiting for the door to close already
    waitToClose = true;                                 // Sets the variable (used to let the system know if it's supposed to wait for the door to close) to true
    debugSerial.println("Door opened...");              // Writes to Serial so that you can see what's going on
  }
  if (doorState == false && waitToClose == true) {      // Runs the code below only if the door is closed AND the system was waiting for the door to close
    debugSerial.println("Door closed.");                // Writes to Serial so that you can see what's going on
    visitCount++;                                       // Increments the value of "visitCount", which is the number of visitors (how many times the door has been opened+closed)
    debugSerial.print("Visitors: ");                    // Writes to Serial so that you can see what's going on
    debugSerial.println(visitCount);                    // Writes to Serial so that you can see what's going on
    payload.reset();                                    // Resets the payload in case there's anything left
    payload.set("15", visitCount);                      // Sets the payload "visitCount" with our entity name "15" (Integer Sensor)
    modem.send(payload);                                // Sends the payload via LoRaWAN
    waitToClose = false;                                // Tells the system not to wait for the door to close anymore
  }
}

void loop() {                                           // The loop function runs in loop as long as the device is powered on
  pushButton();                                         // Runs our 'pushButton' function above
  countVisits();                                        // Runs our 'countVisits' function above
  delay(100);                                           // So we're not running at full speed
}
