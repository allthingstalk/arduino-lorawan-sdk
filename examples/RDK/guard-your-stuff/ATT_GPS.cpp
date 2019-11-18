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
 
#include "ATT_GPS.h"
//#include <SoftwareSerial.h>
#include <SPI.h>
 
/****
* Constructor
*/
ATT_GPS::ATT_GPS(int p1,int p2) : SoftSerial(SoftwareSerial(p1,p2))
{
  SoftSerial.begin(9600);  // reading GPS values from debugSerial connection with GPS
}

// try to read the gps coordinates from the text stream that was received from the gps module
// returns true when gps coordinates were found in the input, otherwise false
bool ATT_GPS::readCoordinates()
{
  // sensor can return multiple types of data
  // we need to capture lines that start with $GPGGA
  bool foundGPGGA = false;
  if (SoftSerial.available())                     
  {
    while(SoftSerial.available())  // read data into char array
    {
      buffer[count++]=SoftSerial.read();  // store data in a buffer for further processing
      if(count == 64)
        break;
    }
    foundGPGGA = count > 60 && extractValues();  // if we have less then 60 characters, we have incomplete input
    clearBufferArray();
  }
  return foundGPGGA;
}

// extract all the coordinates from the stream
// store the values in the globals defined at the top of the sketch
bool ATT_GPS::extractValues()
{
  unsigned char start = count;
  
  // find the start of the GPS data
  // if multiple $GPGGA appear in 1 line, take the last one
  while(buffer[start] != '$')
  {
    if(start == 0)  // it's unsigned char, so we can't check on <= 0
      break;
    start--;
  }
  start++;  // skip the '$', don't need to compare with that

  // we found the correct line, so extract the values
  if(start + 4 < 64 && buffer[start] == 'G' && buffer[start+1] == 'P' && buffer[start+2] == 'G' && buffer[start+3] == 'G' && buffer[start+4] == 'A')
  {
    start += 6;
    timestamp = extractValue(start);
    latitude = convertDegrees(extractValue(start) / 100);
    start = skip(start);    
    longitude = convertDegrees(extractValue(start)  / 100);
    start = skip(start);
    start = skip(start);
    start = skip(start);
    start = skip(start);
    altitude = extractValue(start);
    return true;
  }
  else
    return false;
}

float ATT_GPS::convertDegrees(float input)
{
  float fractional = input - (int)input;
  return (int)input + (fractional / 60.0) * 100.0;
}

// extracts a single value out of the stream received from the device and returns this value
float ATT_GPS::extractValue(unsigned char& start)
{
  unsigned char end = start + 1;
  
  // find the start of the GPS data
  // if multiple $GPGGA appear in 1 line, take the last one
  while(end < count && buffer[end] != ',')
    end++;

  // end the string so we can create a string object from the sub string
  // easy to convert to float
  buffer[end] = 0;
  float result = 0.0;

  if(end != start + 1)  // if we only found a ',' then there is no value
    result = String((const char*)(buffer + start)).toFloat();

  start = end + 1;
  return result;
}

// skip a position in the text stream that was received from the gps
unsigned char ATT_GPS::skip(unsigned char start)
{
  unsigned char end = start + 1;
  
  // find the start of the GPS data
  // if multiple $GPGGA appear in 1 line, take the last one
  while(end < count && buffer[end] != ',')
    end++;

  return end+1;
}

// reset the entire buffer back to 0
void ATT_GPS::clearBufferArray()
{
  for (int i=0; i<count;i++)
  {
    buffer[i]=NULL;
  }
  count = 0;
}

/****
 * Function to calculate the distance between two points
 */
float ATT_GPS::calcDistance(float flat1, float flon1)
{
  return calcDistance(latitude, longitude, flat1, flon1);
}
 
float ATT_GPS::calcDistance(float flat1, float flon1, float flat2, float flon2)
{
  float dist_calc = 0;
  float dist_calc2 = 0;
  float diflat = 0;
  float diflon = 0;

  // I've to split all the calculation in several steps.
  diflat = radians(flat2-flat1);
  flat1 = radians(flat1);
  flat2 = radians(flat2);
  diflon = radians((flon2)-(flon1));

  dist_calc = (sin(diflat/2.0)*sin(diflat/2.0));
  dist_calc2 = cos(flat1);
  dist_calc2 *= cos(flat2);
  dist_calc2 *= sin(diflon/2.0);
  dist_calc2 *= sin(diflon/2.0);
  dist_calc += dist_calc2;

  dist_calc = (2*atan2(sqrt(dist_calc),sqrt(1.0-dist_calc)));

  dist_calc *= 6371000.0; // Converting to meters

  //Serial.println(dist_calc);
  return dist_calc;
}