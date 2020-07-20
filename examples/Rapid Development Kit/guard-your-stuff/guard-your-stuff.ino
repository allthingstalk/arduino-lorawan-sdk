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
 * Do you want to get notified when someone is moving things precious to
 * you? This experiment shows how a LoRa device can be used to detect
 * unexpected movement of an object and send out notifications to its
 * owner. Furthermore, it shows how you can track the object using its
 * GPS location.
 *
 * This example uses an Accelerometer module and a GPS Module. It uses the
 * accelerometer module to check for small movement, and if detected, 
 * it sends GPS location to AllThingsTalk. Then it checks if there was a 
 * significant movement between last and current GPS location checks.
 * If there was, it'll continue sending GPS data to AllThingsTalk on 
 * defined intervals until GPS movement falls below threshold.
 */
/***************************************************************************/

#include <AllThingsTalk_LoRaWAN.h>                // Load the AllThingsTalk LoRaWAN SDK
#include <Wire.h>                                 // Library used for I2C communication with the Accelerometer
#include "MMA7660.h"                              // Library for the Accelerometer module
#include "ATT_GPS.h"                              // Library for the GPS module
#include "keys.h"                                 // Load the header file that contains your credentials for LoRaWAN

#define debugSerial       Serial                  // Define the serial interface that's going to be used for Serial monitor (debugging)
#define loraSerial        Serial1                 // Define the serial interface that'll be used for communication with the LoRa module
#define DEBUG_SERIAL_BAUD 57600                   // Define the baud rate for the debugging serial port (used for Serial monitor)
#define ACCEL_THRESHOLD   12                      // Threshold for accelerometer movement
#define MIN_DISTANCE      30.0                    // Minimal distance between two GPS readings before the device starts sending location updates every "GPS_FIX_DELAY" seconds (check below)
#define GPS_FIX_DELAY     30                      // Delay (seconds) between checking gps coordinates

ABPCredentials credentials(DEVADDR, APPSKEY, NWKSKEY);  // Define the credential variables loaded from the keys.h file
LoRaModem modem(loraSerial, debugSerial, credentials);  // Define LoRa modem properties
CborPayload payload;                              // Create object for setting/sending payload to AllThingsTalk
ATT_GPS gps(20,21);                               // Pins on the board that the GPS module is connected to (20 and 21 are the serial pins on Mbili)
MMA7660 accelerometer;                            // Create 'accelerometer' object

bool    moving = false;                           // Variable used to check if the device is moving
float   prevLatitude, prevLongitude;              // Used to save previous location in order to compare to current one
int8_t  prevX,prevY,prevZ;                        // Used to save previous X, Y and Z axis data in order to compare to current one
unsigned long sendNextAt = 0;                     // Variable used as a timer to check when the program is supposed to send location data when triggered

// This function runs only at boot and only once.
void setup() {
  accelerometer.init();                           // Initialize the accelerometer module
  debugSerial.begin(DEBUG_SERIAL_BAUD);           // Initialize the debug serial port (for Serial monitor)  
  while((!debugSerial) && (millis()) < 10000){}   // Wait for the Serial monitor to be open (so you can see all output) and if it isn't open in 10 seconds, run the program anyways
  while (!modem.init()) { delay(1000); }          // Initialize the modem. If it fails, retry every 1 second until it succeeds
  debugSerial.println("");
  debugSerial.println("------- Guard Your Stuff LoRa Example --------");
  debugSerial.println("---- AllThingsTalk - Make IoT Ideas Happen ----");
  debugSerial.println("");
  debugSerial.print("Initializing GPS...");
  readCoordinates();                              // Calls the function defined below
  debugSerial.println("GPS Initialized!");
  // Now we're going to get the current X, Y and Z axis data from the accelerometer
  // and save that as previous accelerometer values, so when we read the data
  // from the accelerometer later, we have something to compare the values to to determine
  accelerometer.getXYZ(&prevX, &prevY, &prevZ);
  debugSerial.println("Ready to guard your stuff");
  debugSerial.println();
}

// Check acceleration and see if it's above the threshold.
// It does this by comparing the current accelerometer data
// with previous values (last time it was run)
bool isAccelerating() {
  int8_t x,y,z;                                   // Create three variables where the current data will be stored
  accelerometer.getXYZ(&x, &y, &z);               // Read accelerometer data and save it to the variables create above
  // We now need a way to compare the previous reading with the current one. 
  // We'll do this by subtracting current value of X, Y and Z from their old values,
  // then we'll add the results of each one and check if the number is greater than
  // our set threshold (ACCEL_THRESHOLD). Since accelerometer module outputs numbers 
  // that can go negative, we'll use the built-in arduino function "abs" to calculate 
  // absolute values of these numbers.
  bool result = (abs(prevX - x) + abs(prevY - y) + abs(prevZ - z)) > ACCEL_THRESHOLD;
  if(result == true) {                            // If motion is above threshold, save the current values as previous values for future comparison
    Serial.println("Accelerometer motion detected!");
    prevX = x;
    prevY = y;
    prevZ = z;
  }
  return result;                                  // This whole function will return boolean true/false depending on whether motion is detected 
}

// Reads coordinates from the GPS module, and if the GPS module doesn't return anything,
// it waits forever until the module outputs GPS data.
void readCoordinates() {
  while(gps.readCoordinates() == false) {
    debugSerial.print(".");
    delay(1000);
  }
  debugSerial.println();
}

// Sends coordinates to AllThingsTalk along with true/false boolean value that
// represents movement of the device.
void sendCoordinates(boolean val) {
  payload.reset();                                // Resets the payload in case anything is left from before
  payload.set("1", val);                          // Sends true/false value (movement) to AllThingsTalk asset named "1", which is the "Binary sensor"
  
  GeoLocation geoLocation(gps.latitude, gps.longitude, gps.altitude);   // Gets the current GPS coordinates
  payload.set("9", geoLocation);                  // Sets payload with value "geoLocation" to be sent to asset named "9", which is "GPS" asset on AllThingsTalk maker
  
  debugSerial.print("Longitude: ");
  debugSerial.print(gps.longitude, 4);
  debugSerial.print(", Latitude: ");
  debugSerial.print(gps.latitude, 4);
  debugSerial.print(", Altitude: ");
  debugSerial.print(gps.altitude);
  debugSerial.print(", Time: ");
  debugSerial.println(gps.timestamp);
  modem.send(payload);                            // Sends the payload to AllThingsTalk
}

// This function runs in loop forever as long as the device is turned on.
// When device movement is detected through the accelerometer, the current
// location is sent to the platform together with movement state. After
// movement is confirmed using the difference in reported gps position,
// gps data is repeatedly sent (in intervals specified by "GPS_FIX_DELAY")
// to AllThingsTalk Maker until movement stops, which is also detected with gps. 
void loop() {
  if (!moving) {                                  // If not moving, check accelerometer
    moving = isAccelerating();                    // "moving" inherits true/false value from isAccelerating function
    delay(500);
  }
  if(moving && sendNextAt < millis()) {           // If the device is not moving AND it's been more than "GPS_FIX_DELAY* seconds, execute the code below
    readCoordinates();
    if(gps.calcDistance(prevLatitude, prevLongitude) <= MIN_DISTANCE) { // We did not move much (GPS-wise). Back to checking accelerometer for movement.
      debugSerial.print("Less than ");
      debugSerial.print(MIN_DISTANCE);
      debugSerial.print(" movement in last");
      debugSerial.print(GPS_FIX_DELAY);
      debugSerial.println(" seconds");
      moving = false;
      sendCoordinates(false);                    // Sends coordinates along with "false" that basically stands for 'GPS Movement was below threshold'
    } else {                                     // We did move (GPS-wise) above the threshold! Update and send new coordinates
      prevLatitude = gps.latitude;               // Save the current latitude as previous (for next comparison)
      prevLongitude = gps.longitude;             // Save the current longitude as previous (for next comparison)
      sendCoordinates(true);                     // Sends coordinates along with "true" that stands for 'GPS Movement is above threshold!'
    }
    sendNextAt = millis() + GPS_FIX_DELAY*1000;  // Update time (adds GPS_FIX_DELAY in milliseconds to millis() which is time since boot (in milliseconds)
  }
}
