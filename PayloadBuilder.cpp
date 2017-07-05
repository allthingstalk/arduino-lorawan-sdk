/*
  Copyright 2015-2017 AllThingsTalk

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  
  Original author: Peter Leemans
*/

#include "PayloadBuilder.h"

// initialize the payload buffer with the given maximum size
ATT_PB::ATT_PB(uint8_t size) : maxsize(size)
{
  buffer = (uint8_t*) malloc(size);
  cursor = 0;
}

ATT_PB::~ATT_PB(void)
{
  free(buffer);
}

// reset the payload, to call before building a frame payload
void ATT_PB::reset(void)
{
  cursor = 0;
}

// return the current size of the payload
uint8_t ATT_PB::getSize(void)
{
  return cursor;
}

// return the payload buffer
uint8_t* ATT_PB::getBuffer(void)
{
  return buffer;
}

uint8_t ATT_PB::copy(uint8_t* dst)
{
  memcpy(dst, buffer, cursor);
  return cursor;
}


uint8_t ATT_PB::addBoolean(uint8_t value)
{
  if ((cursor + ATTALK_BOOLEAN_SIZE) > maxsize) {
    return 0;
  }
  buffer[cursor++] = value;

  return cursor;
}

uint8_t ATT_PB::addInteger(int value)
{
  if ((cursor + ATTALK_INTEGER_SIZE) > maxsize) {
    return 0;
  }
  byte* fb = (byte*) &value;

  buffer[cursor++] = fb[0];
  buffer[cursor++] = fb[1];

  return cursor;
}

uint8_t ATT_PB::addNumber(float value)
{
  if ((cursor + ATTALK_NUMBER_SIZE) > maxsize) {
    return 0;
  }
  byte* fb = (byte*) &value;

  buffer[cursor++] = fb[0];
  buffer[cursor++] = fb[1];
  buffer[cursor++] = fb[2];
  buffer[cursor++] = fb[3];

  return cursor;
}

uint8_t ATT_PB::addGPS(float latitude, float longitude, float altitude)
{
  if ((cursor + ATTALK_GPS_SIZE) > maxsize) {
    return 0;
  }

  byte* fb = (byte*) &latitude;

  buffer[cursor++] = fb[0];
  buffer[cursor++] = fb[1];
  buffer[cursor++] = fb[2];
  buffer[cursor++] = fb[3];

  fb = (byte*) &longitude;

  buffer[cursor++] = fb[0];
  buffer[cursor++] = fb[1];
  buffer[cursor++] = fb[2];
  buffer[cursor++] = fb[3];

  fb = (byte*) &altitude;

  buffer[cursor++] = fb[0];
  buffer[cursor++] = fb[1];
  buffer[cursor++] = fb[2];
  buffer[cursor++] = fb[3];
  
  return cursor;
}

uint8_t ATT_PB::addAccelerometer(float x, float y, float z)
{
  if ((cursor + ATTALK_ACCEL_SIZE) > maxsize) {
    return 0;
  }

  byte* fb = (byte*) &x;

  buffer[cursor++] = fb[0];
  buffer[cursor++] = fb[1];
  buffer[cursor++] = fb[2];
  buffer[cursor++] = fb[3];

  fb = (byte*) &y;

  buffer[cursor++] = fb[0];
  buffer[cursor++] = fb[1];
  buffer[cursor++] = fb[2];
  buffer[cursor++] = fb[3];

  fb = (byte*) &z;

  buffer[cursor++] = fb[0];
  buffer[cursor++] = fb[1];
  buffer[cursor++] = fb[2];
  buffer[cursor++] = fb[3];
  
  return cursor;
}