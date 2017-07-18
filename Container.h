/*    _   _ _ _____ _    _              _____     _ _     ___ ___  _  __
 *   /_\ | | |_   _| |_ (_)_ _  __ _ __|_   _|_ _| | |__ / __|   \| |/ /
 *  / _ \| | | | | | ' \| | ' \/ _` (_-< | |/ _` | | / / \__ \ |) | ' <
 * /_/ \_\_|_| |_| |_||_|_|_||_\__, /__/ |_|\__,_|_|_\_\ |___/___/|_|\_\
 *                             |___/
 *
 * Copyright 2017 AllThingsTalk
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

#ifndef Container_h
#define Container_h

#include <string.h>
#include <Stream.h>
#include <ContainerPacket.h>

/**
 * A helper class for sending data in the proximus-container data format.
 *
 * All data transmissions are performed using the ATTDevice buffer.
 *
 * To use:
 * - create an instance of the class, and provide the device it should work with.
 * - use one of the Send() methods to send a data value to the cloud.
 *
 * For more info on containers, check
 *   http://docs.allthingstalk.com/developers/data/default-payload-conversion
 */
class Container: public ContainerPacket
{
  public:
    /**
     * Create the object.
     *
     * @param device the buffer object to use for transmitting data
     */
    Container(ATTDevice &device);

    /**
     * Send a bool data value to the cloud server for the sensor with the specified id.
     *
     * @param value :boolean:
     * @param id the container id
     * @param ack :boolean: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for
     *
     * @return true upon success
     */
    bool addToQueue(bool value, short id, bool ack = true);

    /**
     * Send an integer value to the cloud server for the sensor with the specified id.
     *
     * @param value :integer (short):
     * @param id the container id
     * @param ack :boolean: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for
     *
     * @return true upon success
     */
    bool addToQueue(short value, short id, bool ack = true);

    /**
     * Send a string data value to the cloud server for the sensor with the specified id.
     *
     * @param value :string:
     * @param id the container id
     * @param ack :boolean: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for
     *
     * @return true upon success
     */
    bool addToQueue(String value, short id, bool ack = true);

    /**
     * Send a float value to the cloud server for the sensor with the specified id.
     *
     * @param value :float (16 bit):
     * @param id the container id
     * @param ack :boolean: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for
     *
     * @return true upon success
     */
    bool addToQueue(float value, short id, bool ack = true);

    /**
     * Send x, y, z data values to the cloud server for the sensor with the specified id.
     *
     * @param x :float: representing the x coordinate
     * @param y :float: representing the y coordinate
     * @param z :float: representing the z coordinate
     * @param id the container id
     * @param ack :boolean: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for
     *
     * @return true upon success
     */
    bool addToQueue(float x, float y, float z, short id, bool ack = true);

    /**
     * Send data value to the cloud server for the sensor with the specified id. (x, y, z, time values).
     *
     * @param x :float: representing the x coordinate
     * @param y :float: representing the y coordinate
     * @param z :float: representing the z coordinate
     * @param time :float: representing the time at which the coordinates were measured
     * @param id the container id
     * @param ack :boolean: when true, acknowledgement is requested from the base station, otherwise no acknowledge is waited for
     *
     * @return true upon success
     */
    bool addToQueue(float x, float y, float z, float time, short id, bool ack = true);

    /**
     * Extract the data and container type from a downlink message (message from cloud to device).
     *
     * @param data a pointer to the data structure that contains the raw payload
     * @param id a reference to a variable that will receive the id of the container type contained in the message
     *
     * @return the address to the first byte of the data stream. It is up to the client to interprete the pointer according to the data type normally expected for the specified container id
     */
    const uint8_t* parse(const uint8_t* data, short& id);

  protected:

    /**
     * Writes the packet content to the specified byte array. This must be at least 51 bytes long
     *
     * @return the nr of bytes actually written to the array
     */
    virtual unsigned char write(unsigned char* result);

    /**
     * Reset the content of the packet back to 0 ->> all data will be removed
     */
    void reset();

    /**
     * Get the data size of the packet
     */
    unsigned char getDataSize();

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

    /**
     * Send a bool data value to the cloud server for the sensor with the specified id
     * If ack = true -> request acknolodge, otherwise no acknowledge is waited for
     */
    bool add(bool value);

    /**
     * Send an integer value to the cloud server for the sensor with the specified id
     * If ack = true -> request acknowledge, otherwise no acknowledge is waited for
     */
    bool add(short value);

    /**
     * Send a string data value to the cloud server for the sensor with the specified id
     * If ack = true -> request acknowledge, otherwise no acknowledge is waited for
     */
    bool add(String value);

    /**
     * Send a float data value to the cloud server for the sensor with the specified id
     * If ack = true -> request acknowledge, otherwise no acknowledge is waited for
     */
    bool add(float value);
};

#endif