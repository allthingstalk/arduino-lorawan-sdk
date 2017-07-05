/*
  Copyright 2015-2017 AllThingsTalk

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*/

#ifndef ATT_PB_H_
#define ATT_PB_H_

#include <Arduino.h>

// Data Type + Data Size
#define ATTALK_BOOLEAN_SIZE 1
#define ATTALK_INTEGER_SIZE 2
#define ATTALK_NUMBER_SIZE  4
#define ATTALK_BYTE_SIZE    1
#define ATTALK_GPS_SIZE     12
#define ATTALK_ACCEL_SIZE   12


// ATTALK Payload Builder

class ATT_PB {
  
  public:
  
    // Initialize the payload buffer with the given maximum size.
    ATT_PB(uint8_t size);
    ~ATT_PB();

    /**
    Reset the payload, to call before building a frame payload
    */
    void reset(void);

    /**
    Returns the current size of the payload
    */
    uint8_t getSize(void);

    /**
    Return the payload buffer
    */
    uint8_t* getBuffer(void);

    /**
    Copies the payload buffer
    */
    uint8_t copy(uint8_t* buffer);

    /**
    Adds a Boolean to the payload buffer

    parameters:
    - value: can be 0 or 1, representin the boolean

    returns: size of the payload or 0 when max payload size has been exceeded
    */
    uint8_t addBoolean(uint8_t value);

    /**
    Adds an Integer to the payload buffer

    parameters:
    - value: can be a range of -32,768 to 32,767 (2-byte) value

    returns: length of the payload or 0 when max payload size has been exceeded
    */
    uint8_t addInteger(int value);

    /**
    Adds a Number (Float) to the payload buffer

    parameters:
    - value: can be a range between  3.4028235E+38 and -3.4028235E+38. (4 bytes) value

    returns: length of the payload or 0 when max payload size has been exceeded
    */
    uint8_t addNumber(float value);

    /**
    Adds a GPS object to the payload buffer

    parameters:
    - latitude : float : can be a range between 3.4028235E+38 and -3.4028235E+38. (4 bytes) value
    - longitude : float : can be a range between 3.4028235E+38 and -3.4028235E+38. (4 bytes) value
    - altitude : float : can be a range between 3.4028235E+38 and -3.4028235E+38. (4 bytes) value

    returns: length of the payload or 0 when max payload size has been exceeded
    */
    uint8_t addGPS(float latitude, float longitude, float altitude);

    /**
    Adds an addAccelerometer object to the payload buffer

    parameters:
    - x : float : can be a range between 3.4028235E+38 and -3.4028235E+38. (4 bytes) value
    - y : float : can be a range between 3.4028235E+38 and -3.4028235E+38. (4 bytes) value
    - z : float : can be a range between 3.4028235E+38 and -3.4028235E+38. (4 bytes) value

    returns: length of the payload or 0 when max payload size has been exceeded
    */
    uint8_t addAccelerometer(float x, float y, float z);

  private:

    uint8_t *buffer;
    uint8_t maxsize;
    uint8_t cursor;
};

#endif
