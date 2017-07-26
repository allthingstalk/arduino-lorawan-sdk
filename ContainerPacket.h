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

#ifndef ContainerPacket_h
#define ContainerPacket_h


#define BINARY_SENSOR ((short)1)
#define BINARY_TILT_SENSOR ((short)2)
#define PUSH_BUTTON ((short)3)
#define DOOR_SENSOR ((short)4)
#define TEMPERATURE_SENSOR ((short)5)
#define LIGHT_SENSOR ((short)6)
#define PIR_SENSOR ((short)7)
#define ACCELEROMETER ((short)8)
#define GPS ((short)9)
#define PRESSURE_SENSOR ((short)10)
#define HUMIDITY_SENSOR ((short)11)
#define LOUDNESS_SENSOR ((short)12)
#define AIR_QUALITY_SENSOR ((short)13)
#define BATTERY_LEVEL ((short)14)
#define INTEGER_SENSOR ((short)15)
#define NUMBER_SENSOR ((short)16)

#include <Stream.h>
#include <ATT_IOT_LoRaWAN.h>

/*
 * Base class for supported data formats
 */
class ContainerPacket
{
  public:
    /**
     * Create the object
     *
     * @param device the device that this packet will transmit data through
     */
    ContainerPacket(ATTDevice &device);

    /**
     * Get a reference to the device object
     */
    ATTDevice* getDevice() {return _device;};

  protected:
    ATTDevice* _device;

    /**
     * Assign the asset/container id to the packet.
     */
    void setId(unsigned char id);

    /**
     * Reset the content of the packet back to 0 ->> all data will be removed.
     */
    virtual void reset() = 0;

    /**
     * Get the data size of the packet.
     */
    virtual unsigned char getDataSize() = 0;

    /**
     * Write the packet content to the specified byte array.
     * This must be max 51 to 220 bytes long, depending on spreading factor.
     *
     * @return the nr of bytes actually written to the array
     */
    virtual unsigned char write(unsigned char* result);

    /**
     * Return the frame type number for this lora packet.
     * The default value is 0x40. Inheritors that render other packet types can overwrite this.
     */
    virtual unsigned char getFrameType();

    /**
     * Calculate the checksum of the packet and return it.
     */
    unsigned char calculateCheckSum(unsigned char* toSend, short len);

  private:
    unsigned char contId;
};

#endif