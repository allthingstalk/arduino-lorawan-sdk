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
 * WHAT DOES THIS SKETCH DO
 * ------------------------
 * Track the device and send the coordinates to the backend every x seconds
 * 
 * MEANING OF LIGHTS
 * -----------------
 * Blue: making connection with networking
 * Green: (blinking) connection successful, (normal) sending successful
 * Red: (blinking) connection failed, (normal) sending failed
 * Yellow: getting gps coordinates
 * White: sending payload to backend
 */

#include <AllThingsTalk_LoRaWAN.h>
#include <Sodaq_UBlox_GPS.h>

#include "keys.h"

#define debugSerial SerialUSB
#define loraSerial Serial1

ABPCredentials credentials(DEV_ADDR, APPSKEY, NWKSKEY);
LoRaModem modem(loraSerial, debugSerial, credentials);

CborPayload payload;

const int GPS_TIMEOUT = 60000;

enum lightColor {
  red,
  green,
  blue,
  yellow,
  magenta,
  cyan,
  white,
  off
};

void initiateLights()
{
  //initiating light pins
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);
}

void setLight(lightColor color, bool animate = false, int animateCount = 3)
{
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  digitalWrite(LED_BLUE, HIGH);

  switch (color)
  {
    case red:
      if (animate)
      {
        for (int i = 0; i < animateCount; i++)
        {
          digitalWrite(LED_RED, LOW);
          delay(500);
          digitalWrite(LED_RED, HIGH);
          delay(500);
        }
      }
      digitalWrite(LED_RED, LOW);
      break;

    case green:
      if (animate)
      {
        for (int i = 0; i < animateCount; i++)
        {
          digitalWrite(LED_GREEN, LOW);
          delay(500);
          digitalWrite(LED_GREEN, HIGH);
          delay(500);
        }
      }
      digitalWrite(LED_GREEN, LOW);
      break;

    case blue:
      if (animate)
      {
        for (int i = 0; i < animateCount; i++)
        {
          digitalWrite(LED_BLUE, LOW);
          delay(500);
          digitalWrite(LED_BLUE, HIGH);
          delay(500);
        }
      }
      digitalWrite(LED_BLUE, LOW);
      break;  

    case yellow:
      if (animate)
      {
        for (int i = 0; i < animateCount; i++)
        {
          digitalWrite(LED_RED, LOW);
          digitalWrite(LED_GREEN, LOW);
          delay(500);
          digitalWrite(LED_RED, HIGH);
          digitalWrite(LED_GREEN, HIGH);
          delay(500);
        }
      }
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, LOW);
      break;

    case magenta:
      if (animate)
      {
        for (int i = 0; i < animateCount; i++)
        {
          digitalWrite(LED_RED, LOW);
          digitalWrite(LED_BLUE, LOW);
          delay(500);
          digitalWrite(LED_RED, HIGH);
          digitalWrite(LED_BLUE, HIGH);
          delay(500);
        }
      }
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_BLUE, LOW);
      break;

    case cyan:
      if (animate)
      {
        for (int i = 0; i < animateCount; i++)
        {
          digitalWrite(LED_GREEN, LOW);
          digitalWrite(LED_BLUE, LOW);
          delay(500);
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_BLUE, HIGH);
          delay(500);
        }
      }
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, LOW);
      break;

    case white:
      if (animate)
      {
        for (int i = 0; i < animateCount; i++)
        {
          digitalWrite(LED_RED, LOW);
          digitalWrite(LED_GREEN, LOW);
          digitalWrite(LED_BLUE, LOW);
          delay(500);
          digitalWrite(LED_RED, HIGH);
          digitalWrite(LED_GREEN, HIGH);
          digitalWrite(LED_BLUE, HIGH);
          delay(500);
        }
      }
      digitalWrite(LED_RED, LOW);
      digitalWrite(LED_GREEN, LOW);
      digitalWrite(LED_BLUE, LOW);
      break;
  }
}

void setup() {
  debugSerial.begin(57600);
  while (!debugSerial && millis() < 10000) {}

  initiateLights();

  debugSerial.println("Init modem");
  
  setLight(blue);
  if (!modem.init())
  {
    setLight(red, true, 10);
    exit(0);
  }
  else
  {
    setLight(green, true);
    
    debugSerial.println("Init succeeded");
    debugSerial.println("Init GPS");
    sodaq_gps.init(GPS_ENABLE);
    
    LoRaOptions options;
    options.port = 1;
    
    modem.setOptions(options);
  }
}

void loop() {
  setLight(yellow);
  if (sodaq_gps.scan(false, GPS_TIMEOUT))
  {
    debugSerial.println("GPS Fix!");
    double latitude = sodaq_gps.getLat();
    double longitude = sodaq_gps.getLon();
    double altitude = 0;
  
    GeoLocation geoLocation(latitude, longitude, altitude);
  
    setLight(white);
    payload.reset();
    payload.set("loc", geoLocation);
  
    if (modem.send(payload))
      setLight(green);
    else
      setLight(red);

    delay(1000);
    setLight(off);
  
    delay(30000);
  }
}
