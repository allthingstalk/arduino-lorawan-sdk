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

#ifndef LoRaModem_h
#define LoRaModem_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

#include <instrumentationParamEnum.h>

#define SENDSTATE_TRANSMITCOMMAND 0  // 3 states used to track async send status
#define SENDSTATE_EXPECTOK 1
#define SENDSTATE_GETRESPONSE 2
#define SENDSTATE_DONE 3

#define MAX_PAYLOAD_SIZE 222      // maximum allowed size for a payload
#define SMALLEST_PAYLOAD_SIZE 51  // the payload size for the biggest sf


// callback signature for functions that process data coming from the NSP that were send to the device
// first param : start of byte array that was found
// second param: the length of the package
// third param: port
#define ATT_CALLBACK_SIGNATURE void (*callback)(const uint8_t*,unsigned int, uint8_t)

/*
 * Base class for modems
 */
class LoRaModem
{
  public:
    /**
     * Create the modem object.
     *
     * @param monitor a stream object, used to write output data towards
     * @param MQTT_CALLBACK_SIGNATURE assign a callback function that is called when incoming data (from nsp to device) needs to be processed. Null by default, so no callback will be performed
     */
    LoRaModem(Stream *monitor, ATT_CALLBACK_SIGNATURE = NULL);

    /**
     * Return the required baudrate for the device
     *
     * @return an unsigned integer, representing the default baut rate
     */
    virtual unsigned int getDefaultBaudRate() = 0;

    /**
     * Stop the modem.
     *
     * @return true upon success
     */
    virtual bool stop() = 0;

    /**
     * Set the modem in LoRaWan mode (versus private networks).
     *
     * @param adr when true, use adaptive data rate (default)
     *
     * @return true upon success
     */
    virtual bool setLoRaWan(bool adr = true) = 0;

    /**
     * Assign a device address to the modem.
     *
     * @param devAddress the device address to used. Must be 4 bytes long
     *
     * @return true upon success
     */
    virtual bool setDevAddress(const unsigned char* devAddress) = 0;

	/**
     * Assign a device address to the modem.
     *
     * @param devAddress the device address to used. Must be 8 bytes long
     *
     * @return true upon success
     */
    virtual bool setDevEUI(const unsigned char* devEUI) = 0;

    /**
     * Set the app session key for the modem communication.
     *
     * @param appskey the app session key, must be 16 bytes long
     *
     * @return true upon success
     */
    virtual bool setAppsKey(const unsigned char* appsKey) = 0;

	/**
     * Set the app key for the modem communication.
     *
     * @param appkey the app session key, must be 16 bytes long
     *
     * @return true upon success
     */
    virtual bool setAppKey(const unsigned char* appKey) = 0;

    /** set the network session key
     *
     * @param nwksKey the network session key, must be 16 bytes long
     *
     * @return true upon success
     */
    virtual bool setNWKSKey(const unsigned char*  nwksKey) = 0;

	/**
     * Assign a apps EUI address to the modem.
     *
     * @param appsEUI the apps eui address to used. Must be 8 bytes long
     *
     * @return true upon success
     */
    virtual bool setAppEUI(const unsigned char* appsEUI) = 0;

    /**
     * Start the modem.
     *
     * @return true upon success
     */
    virtual bool start(const char* activation) = 0;

    /**
     * Send a data packet to the NSP.
     * This operation is performed synchronically. If ack is requested, then the function will block until the base station has responded or the time out has expired.
     *
     * @param data the byte array or pointer to a structure that needs to be sent
     * @param size the nr of bytes in the data block
     * @param ack when true, an acknowledge is request fromo the base station (default), otherwise no acknowledge is waited for
     *
     * @return true upon success
     */
    virtual bool send(void* packet, unsigned char size, bool ack = true);

    /**
     * Start the send process, but return before everything is done.
     * This operation is performed asynchronically. If an ack is requested, then the operation is not yet complete when this function returns.
     * Consecutive `checkSendState()` calls should be performed untill the operation has been completed.
     *
     * @return true if the packet was succesfully send, and the process of waiting for a resonse can begin. Otherwise return false
     */
    virtual bool sendAsync(void* packet, unsigned char size, bool ack = true, uint8_t port = 1) = 0;

    /**
     * Checks the status of the current send operation (if there was any).
     *
     * @param sendResult the result of the send operation, if there was still a pending operation
     *
     * @return if there was none or the operation is done
     */
    virtual bool checkSendState(bool& sendResult) = 0;

    /**
     * @return true if the modem can send a payload. False otherwise
     */
    inline bool isFree(){ return sendState == SENDSTATE_DONE; };

    /**
     * Process any incoming packets from the modem.
     */
    virtual void processIncoming() = 0;

    /**
     * Extract the specified instrumentation parameter from the modem and return the value.
     *
     * @param param the id of the parameter whose value should be returned
     *
     * @return the value of the specified parameter
     */
    virtual int getParam(instrumentationParam param) = 0;

    /**
     * @return the id number of the modem type
     */
    virtual int getModemId() = 0;

    /**
     * Calcualte the max payload size, based on the current spreading factor of the modem. Used to check if the packet can be sent.
     *
     * @param spreading_factor the spreading factor to calculate the max payload size for
     *
     * @return the maximum size of the payload for the given spreading factor
     */
    int maxPayloadForSF(short spreading_factor = -1);

    /**
     * Calcualte the time on air, based on the current spreading factor and payload size.
     *
     * @param appPayloadSize the size of the payload to send
     * @param spreading_factor the spreading factor to calculate the max payload size for
     *
     * @return the time on air
     */
    float calculateTimeOnAir(unsigned char appPayloadSize, short spreading_factor = -1);

    /**
     * Get the current state of the (async) send operation.
     */
    char getSendState() {return sendState;};

  protected:
    Stream *_monitor;
    void (*_callback)(const uint8_t*,unsigned int, uint8_t);
    // keeps track of the current async send position: are we waiting for 'ok' or response
    char sendState;

    // stores the previous payload size. This is for in case that the payload size is requested while we are waiting on a response from
    // the modem. In this case, we can't request the sf from the modem, so we can't calculate the value. Instead we use this buffered val
    int _prevPayloadForSF;

    void printHex(unsigned char hex);

    float calculateSymbolTime(short spreading_factor = -1, short bandwidth = -1) ;
    int calculateSymbolsInPayload(unsigned char appPayloadSize, short spreading_factor) ;

};

#endif
