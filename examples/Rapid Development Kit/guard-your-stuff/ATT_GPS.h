/*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
 *   /_\ | | |_   _| |_ (_)_ _  __ _ __|_   _|_ _| | |__ / __|   \| |/ /
 *  / _ \| | | | | | ' \| | ' \/ _` (_-< | |/ _` | | / / \__ \ |) | ' <
 * /_/ \_\_|_| |_| |_||_|_|_||_\__, /__/ |_|\__,_|_|_\_\ |___/___/|_|\_\
 *                             |___/
 *
 * Copyright 2018 AllThingsTalk
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
 
#include <SoftwareSerial.h>
 
class ATT_GPS {

  public:
    // Constructors
    ATT_GPS(int p1,int p2);
    
    bool readCoordinates();
    float convertDegrees(float input);
    float calcDistance(float flat1, float flon1, float flat2, float flon2);
    float calcDistance(float flat1, float flon1);  // distance to current coordinates
    void setStream(SoftwareSerial& SoftSerial);
    
    // Variables for the coordinates (GPS)
    float latitude;
    float longitude;
    float altitude;
    float timestamp;
    
  private:
    bool extractValues();
    float extractValue(unsigned char& start);
    unsigned char skip(unsigned char start);
    void clearBufferArray();
    SoftwareSerial SoftSerial;
    unsigned char buffer[64];  // buffer array for data receive over debugSerial port
    int count=0; 
};