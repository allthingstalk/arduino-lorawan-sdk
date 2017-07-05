/*
  Copyright 2015-2017 AllThingsTalk

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  
  Original author: Jan Bogaerts
*/

#ifndef Container_h
#define Container_h

#include <string.h>
#include <Stream.h>
#include <ContainerPacket.h>

/**
 A helper class for sending data in the proximus-container data format.

 All data transmissions are performed using the ATTDevice buffer.

 To use:
 - create an instance of the class, and provide the device it should work with.
 - use one of the Send() methods to send a data value to the cloud.
*/
class Container: public ContainerPacket
{
  public:
    /**
    Create the object.

    parameters:
    - device: the buffer object to use for transmitting data
    */
    Container(ATTDevice &device);


    /**
    Send a bool data value to the cloud server for the sensor with the specified id.

    parameters:
    - value: a boolean value
    - id: the container id, see [here](http://docs.allthingstalk.com/developers/libraries/lora/#supported-sensors-and-actuators) for more info on the available containers
    - ack: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for

    returns: true upon success.
    */
    bool AddToQueue(bool value, short id, bool ack = true);

    /**
    Send an integer value to the cloud server for the sensor with the specified id.

    parameters:
    - value: an integer (short) value
    - id: the container id, see [here](http://docs.allthingstalk.com/developers/libraries/lora/#supported-sensors-and-actuators) for more info on the available containers
    - ack: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for

    returns: true upon success
    */
    bool AddToQueue(short value, short id, bool ack = true);

    /**
    Send a string data value to the cloud server for the sensor with the specified id.

    parameters:
    - value: a string value
    - id: the container id, see [here](http://docs.allthingstalk.com/developers/libraries/lora/#supported-sensors-and-actuators) for more info on the available containers
    - ack: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for

    returns: true upon success
    */
    bool AddToQueue(String value, short id, bool ack = true);

    /**
    Send a float value to the cloud server for the sensor with the specified id.

    parameters:
    - value: a float (16 bit) value
    - id: the container id, see [here](http://docs.allthingstalk.com/developers/libraries/lora/#supported-sensors-and-actuators) for more info on the available containers
    - ack: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for

    returns: true upon success
    */
    bool AddToQueue(float value, short id, bool ack = true);

    /**
    Send x, y, z data values to the cloud server for the sensor with the specified id.

    parameters:
    - x: a float value, usually representing an x coordinate
    - y: a float value, usually representing an y coordinate
    - z: a float value, usually representing an z coordinate
    - id: the container id, see [here](http://docs.allthingstalk.com/developers/libraries/lora/#supported-sensors-and-actuators) for more info on the available containers
    - ack: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for

    returns: true upon success
    */
    bool AddToQueue(float x, float y, float z, short id, bool ack = true);

    /**
    Send data value to the cloud server for the sensor with the specified id. (x, y, z, time values).

    parameters:
    - x: a float value, usually representing an x coordinate
    - y: a float value, usually representing an y coordinate
    - z: a float value, usually representing an z coordinate
    - time: a float value, usually representing the timestamp at which the coordinates were measured
    - id: the container id, see [here](http://docs.allthingstalk.com/developers/libraries/lora/#supported-sensors-and-actuators) for more info on the available containers
    - ack: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for

    returns: true upon success
    */
    bool AddToQueue(float x, float y, float z, float time, short id, bool ack = true);


    /**
    Extract the data and container type from a downlink message (message from cloud to device).

    parameters:
    - data: a pointer to the data structure that contains the raw payload.
    - id: a reference to a variable that will receive the id of the container type contained in the message.

    returns: the address to the first byte of the data stream. It is up to the client to interprete the pointer according to the data type normally expected for the specified container id
    **/
    const uint8_t* Parse(const uint8_t* data, short& id);
  protected:

    // writes the packet content to the specified byte array. This must be at least 51 bytes long.
    // returns: the nr of bytes actually written to the array.
    virtual unsigned char Write(unsigned char* result);

    // resets the content of the packet back to 0 ->> all data will be removed
    void Reset();

    // get the data size of the packet
    unsigned char GetDataSize();

  private:
    // define the stores for all the values for this packet
    char stringValues[48];
    short intValues[16];
    float floatValues[16];
    unsigned char boolValues;
    unsigned char stringPos;
    unsigned char nrInts;
    unsigned char nrFloats;
    unsigned char nrBools;

    // send a bool data value to the cloud server for the sensor with the specified id
    // if ack = true -> request acknolodge, otherwise no acknowledge is waited for
    bool Add(bool value);

    // send an integer value to the cloud server for the sensor with the specified id
    // if ack = true -> request acknowledge, otherwise no acknowledge is waited for
    bool Add(short value);

    // send a string data value to the cloud server for the sensor with the specified id
    // if ack = true -> request acknowledge, otherwise no acknowledge is waited for
    bool Add(String value);

    // send a float data value to the cloud server for the sensor with the specified id
    // if ack = true -> request acknowledge, otherwise no acknowledge is waited for
    bool Add(float value);
};

#endif
