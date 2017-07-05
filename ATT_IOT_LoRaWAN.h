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
//  Configuration
/////////////////////////////////////////////////////////////
#define SEND_MAX_RETRY 30  // the default max nr of times that 'send' functions will retry to send the same value.
#define MIN_TIME_BETWEEN_SEND 10000  // the default value for the minimum time between 2 consecutive calls to Send data. (expressed in milliseconds)

#define VERSION "2.0"

#define QUEUESIZE 15                  


/////////////////////////////////////////////////////////////


/**
This class provides buffered, asynchronous data transmission features.  

The buffer (a queue) allows the application to gracefully handle situations where the lora network does
not yet allow the device to send data or when the network connection has been temporarily lost.
Delays between messages can be calculated automatically, based on the current transmission speed, or can be set by a fixed value.

The size of the queue is determined by a 'define' called QUEUESIZE. It's default value is 15.

To use this class:
- create an instance and 
- call the Connect() function in order to establish a connection with the NSP. 
- use Send() to send a data packet to the NSP at the earliest convenience. 
- regularly call ProcessQueue() or 'ProcessQueuePopFailed' to make certain that any pending actions are processed.

there are also functions available to manage the buffer:
- `Pop()` remove the first item from the queue
- `IsQueueEmpty()` check if the queue is currently empty (fast method)
- `IsQueueFull()` check if the queue is currently full
- `QueueCount()` get the current nr of elements in the queue

Note: if you want to send data in blocking mode, you can either call 'ProcessQueue' after each 'Send' operation from the queue, or use the
'Send' function of the modem object directly.
*/
class ATTDevice
{
  public:
    /**
    Create the object
    
    parameters:
    - modem: the object that respresents the modem that should be used.
    - monitor: the stream used to write log lines to.
    - autoCalMinTime: when true (default), the minimum time between 2 consecutive messages will be calculated based on the spreading factor, otherwise the default minTime will be used.
    - minTime: if `autoCalMinTime` is false, this value indicates the fixed minimum time between 2 messages. Expressed in milli seconds (default is 10000 millseconds). If `autoCalMinTime` is true, this is the minimum time that the system can't go below (important for some systems where the minimum delay can be 6 sec, but not all nsp's allow this)
    */
    ATTDevice(LoRaModem* modem, Stream* monitor = NULL, bool autoCalMinTime=true, unsigned int minTime=MIN_TIME_BETWEEN_SEND);
    
    /**
    Configure the modem for ABP mode with the device address, app session key and network session key.
        
    parameters:
    - devAddress: the device address, as provided by the NSP
    - appKey: the app session key, as provided by the NSP
    - nwksKey: the network session key, as provided by the NSP
    - adr: when true, adaptive data rate is used (default)
    
    returns: true when the operation was successfully performed, otherwise false
    
    > Even if this function returns true, this does not yet mean you are already in contact with a base station. It only means that the modem was succesfully configured with the provided parameters and an abp request has been made. The library will however automatically try to reconnect using abp mode when there was a problem with the connection.
    */
    bool InitABP(const uint8_t* devAddress, const uint8_t* appKey, const uint8_t*  nwksKey, bool adr = true);

    /**
    Sends the specified payload to the NSP. 
    
    If required (no connection, not enough time between 2 consecutive messages), then the data is buffered until it can be sent.
    The buffer has a maximum size, upon overrun, new messages are discarded. It is your responsibility to handle this (remove the new data or remove data from the queue using Pop())
    
    parameters:
    - data: the byte array or pointer to a structure that needs to be sent
    - size: the nr of bytes in the data block
    - ack: when true, an acknowledge is request fromo the base station (default), otherwise no acknowledge is waited for
    
    returns: true when the packet has been buffered, or the transmission has begun. Use processQueue or ProcessQueuePopFailed to get the result of the transmission
    */
    bool AddToQueue(void* data, unsigned char size, bool ack = true);
    
    /**
    Instructs the system to process any incomming responses from the base station and to try and send a message from it's queue,
    if there are any and if the system is ready for transmission 
      
    If the modem reports a failed transmission, then the system will keep the message in it's buffer and try to resend it in the next time slot.
          
    returns:
    - `0`: no more items on to process, all is done
    - `1`: still items to be processed, call this function again
    - `-1`: the message currently on top failed transmission: if you want to disgard it, remove it manually with pop, otherwise the system 
       will try to resend the payload
    */
    int ProcessQueue();
    
    /**
    removes the current message at the front of the queue, if there is still data in the buffer
    */
    void Pop();
    
    /**
    returns: true if the queue is empty
    */
    inline bool IsQueueEmpty() { return _front == _back; };
    
    /**
    returns: true if the queue is full
    */
    inline bool IsQueueFull() { return _front - 1 == _back  || (_front == 0 && _back == QUEUESIZE - 1); };
    
    /**
    Get the nr of items currently in the queue
    */
    inline unsigned char QueueCount(){ if(_back > _front) return _back - _front; else return _front - _back;};
    
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
    const uint8_t* _appKey;
    const uint8_t* _nwksKey;
    bool _adr;
    
    void Push(void* data, unsigned char size, bool ack = true);
    
    // send data to modem for transmission
    void SendASync(void* data, unsigned char size, bool ack);
    
    // sends the payload at the front of the queue, if there is any and if it's within the allowed time frame
    // returns true if there is still more work to be done. False if there was no more front to be sent
    bool trySendFront();
    
    // check if keys are filled in
    bool HasKeys();
    
    // used for setting up initial connection and trying to reconnect
    bool CheckInitStatus();
};

#endif
