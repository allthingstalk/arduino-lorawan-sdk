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

#ifndef ATTDevice_h
#define ATTDevice_h

//#include "Arduino.h"
#include <string.h>
#include <Stream.h>

#include <LoRaModem.h>
//#include <DataPacket.h>
//#include <instrumentationParamEnum.h>
//#include <InstrumentationPacket.h>

/////////////////////////////////////////////////////////////
//
//  Configuration
//
#define SEND_MAX_RETRY 30  // the default max nr of times that 'send' functions will retry to send the same value.
#define MIN_TIME_BETWEEN_SEND 10000  // the default value for the minimum time between 2 consecutive calls to Send data. (expressed in milliseconds)

#define VERSION "2.0"

#define QUEUESIZE 15                  

/////////////////////////////////////////////////////////////


/**
 * This class provides buffered, asynchronous data transmission features.  
 *
 * The buffer (a queue) allows the application to gracefully handle situations where the lora network does
 * not yet allow the device to send data or when the network connection has been temporarily lost.
 * Delays between messages can be calculated automatically, based on the current transmission speed, or can be set
 * by a fixed value.
 *
 * The size of the queue is determined by a 'define' called QUEUESIZE. It's default value is 15.
 *
 * To use this class:
 * - create an instance and 
 * - call the Connect() function in order to establish a connection with the NSP.
 * - use addToQueue() to add a packet to the queue. It is then sent to the NSP at the earliest convenience.
 * - regularly call ProcessQueue() or 'ProcessQueuePopFailed' to make certain that any pending actions are processed.
 *
 * There are also functions available to manage the buffer:
 * - `pop()` remove the first item from the queue
 * - `isQueueEmpty()` check if the queue is currently empty (fast method)
 * - `isQueueFull()` check if the queue is currently full
 * - `queueCount()` get the current nr of elements in the queue
 */
class ATTDevice
{
  public:
    /**
     * Create the object
     *
     * @param modem the object that respresents the modem that should be used
     * @param monitor the stream used to write log lines to
     * @param autoCalMinTime when true (default), the minimum time between 2 consecutive messages will be calculated based on the spreading factor, otherwise the default minTime will be used
     * @param minTime if `autoCalMinTime` is false, this value indicates the fixed minimum time between 2 messages. Expressed in milli seconds (default is 10000 millseconds). If `autoCalMinTime` is true, this is the minimum time that the system can't go below (important for some systems where the minimum delay can be 6 sec, but not all nsp's allow this)
     */
    ATTDevice(LoRaModem* modem, Stream* monitor = NULL, bool autoCalMinTime=true, unsigned int minTime=MIN_TIME_BETWEEN_SEND);
    
    /**
     * Configure the modem for ABP mode with the device address, app session key and network session key
     *
     * @param devAddress the device address, as provided by the NSP
     * @param appKey the app session key, as provided by the NSP
     * @param nwksKey the network session key, as provided by the NSP
     * @param adr when true, adaptive data rate is used (default)
     *    
     * @return true when the operation was successfully performed, otherwise false
     *
     * > Even if this function returns true, this does not yet mean you are already in contact with a base station. It only means that the modem was succesfully configured with the provided parameters and an abp request has been made. The library will however automatically try to reconnect using abp mode when there was a problem with the connection.
     */
    bool initABP(const uint8_t* devAddress, const uint8_t* appsKey, const uint8_t*  nwksKey, bool adr = true);

    /**
     * Send the specified payload to the NSP
     *
     * If required (no connection, not enough time between 2 consecutive messages), then the data is buffered until it can be sent.
     * The buffer has a maximum size, upon overrun, new messages are discarded. It is your responsibility to handle this (remove the new data or remove data from the queue using Pop())
     *
     * @param data the byte array or pointer to a structure that needs to be sent
     * @param size the nr of bytes in the data block
     * @param ack when true, an acknowledge is request fromo the base station (default), otherwise no acknowledge is waited for
     *
     * @return true when the packet has been buffered, or the transmission has begun. Use processQueue or ProcessQueuePopFailed to get the result of the transmission
     */
    bool addToQueue(void* data, unsigned char size, bool ack = true);
    
    /**
     * Instruct the system to process any incomming responses from the base station and to try and send a message from it's queue, if there are any and if the system is ready for transmission 
     * 
     * If the modem reports a failed transmission, then the system will keep the message in it's buffer and try to resend it in the next time slot.
     *     
     * @return
     * - `0`: no more items on to process, all is done
     * - `1`: still items to be processed, call this function again
     * - `-1`: the message currently on top failed transmission: if you want to disgard it, remove it manually with pop, otherwise the system will try to resend the payload
     */
    int processQueue();
    
    void sendCbor(unsigned char* data, unsigned int size);
    
    /**
     * remove the current message at the front of the queue, if there is still data in the buffer
     */
    void pop();
    
    /**
     * @return true if the queue is empty
     */
    inline bool isQueueEmpty() { return _front == _back; };
    
    /**
     * @return true if the queue is full
     */
    inline bool isQueueFull() { return _front - 1 == _back  || (_front == 0 && _back == QUEUESIZE - 1); };
    
    /**
     * Get the nr of items currently in the queue
     */
    inline unsigned char queueCount(){ if(_back > _front) return _back - _front; else return _front - _back;};
    
  private:  
    unsigned long _minTimeBetweenSend;
    unsigned long _minAllowedTimeBetweenSend;
    unsigned long _lastTimeSent;  // the last time that a message was sent, so we can block sending if user calls send to quickly
    Stream *_monitor;
    LoRaModem* _modem;
    unsigned char _queue[QUEUESIZE][MAX_PAYLOAD_SIZE + 2];  // buffers the data 1 extra byte in the payload for the ack request flag (at end of array -> last byte) + 1 extra byte for the size, next to last byte
    char _front;
    char _back;
    bool _sendFailed;
    bool _autoCalMinTime;
    
    const uint8_t* _devAddress;
    const uint8_t* _appsKey;
    const uint8_t* _nwksKey;
    bool _adr;
    
    void push(void* data, unsigned char size, bool ack = true);
    
    /**
     * Send data to modem for transmission
     */
    void sendASync(void* data, unsigned char size, bool ack);
    
    /**
     * Send the payload at the front of the queue, if there is any and if it's within the allowed time frame
     * @return true if there is still more work to be done. False if there was no more front to be sent
     */
    bool trySendFront();
    
    /**
     * Check if keys are filled in
     */
    bool hasKeys();
    
    /**
     * Set up initial connection and/or trying to reconnect
     */
    bool checkInitStatus();
};

#endif
