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

#include <ATT_IOT_LoRaWAN.h>
#include "Utils.h"
//#include <arduino.h>  // still required for the 'delay' function

// create the object
ATTDevice::ATTDevice(LoRaModem* modem, Stream* monitor, bool autoCalMinTime, unsigned int minTime):
  _minTimeBetweenSend(minTime), 
  _autoCalMinTime(autoCalMinTime), 
  _minAllowedTimeBetweenSend(minTime)
{
  _modem = modem;
  _monitor = monitor;
  _back = _front = _lastTimeSent = 0;
  _sendFailed = false;
}

// connect with the to the lora gateway
bool ATTDevice::initABP(const uint8_t* devAddress, const uint8_t* appsKey, const uint8_t* nwksKey, bool adr)
{
  _devAddress = devAddress;
  _appsKey = appsKey;
  _nwksKey = nwksKey;
  
  if(!hasKeys())
  {
    PRINTLN("Program stopped");
    while(1);
  }
  
  _adr = adr;
  PRINT("ATT lib version: "); PRINTLN(VERSION);
  if(!_modem->stop())  // stop any previously running modems
  {
    PRINTLN("can't communicate with modem: possible hardware issues");
    return false;
  }
  return checkInitStatus();
}

bool ATTDevice::hasKeys()
{
  // check if keys are filled in
  int sum[3] = {0,0,0};
  for(int i=0; i<sizeof(_devAddress); i++)
  {
    sum[0] += _devAddress[i];
  }
  for(int i=0; i<sizeof(_appsKey); i++)
  {
    sum[1] += _appsKey[i];
  }
  for(int i=0; i<sizeof(_nwksKey); i++)
  {
    sum[2] += _nwksKey[i];
  }
  if(sum[0] == 0 || sum[1] == 0 || sum[2] == 0){
    PRINTLN("Please fill in DEV_ADDR, APPSKEY and NWKSKEY in your keys.h file");
    return false;
  }
  
  return true;
}

bool ATTDevice::checkInitStatus()
{
  if (!_modem->setLoRaWan(_adr)){  // switch to LoRaWAN mode instead of peer to peer        
    PRINTLN("can't set adr: possible hardware issues?");
    return false;
  }
  if(!_modem->setDevAddress(_devAddress)){
    PRINTLN("can't assign device address to modem: possible hardware issues?");
    return false;
  }
  if(!_modem->setAppsKey(_appsKey)){
    PRINTLN("can't assign app session key to modem: possible hardware issues?");
    return false;
  }
  if(!_modem->setNWKSKey(_nwksKey)){
    PRINTLN("can't assign network session key to modem: possible hardware issues?");
    return false;
  }
  bool result = _modem->start();  // start up the modem
  if(result == true){
    PRINTLN("modem initialized");
  }
  else{
    PRINTLN("Parameters loaded, but modem won't start: initialization failed");
  }
  return result;
}


// try to send the front message (oldest) in the queue
bool ATTDevice::trySendFront()
{
  if(isQueueEmpty() == false){
    unsigned long curTime = millis();
    unsigned long timeoutAt = _lastTimeSent + _minTimeBetweenSend;
    if(timeoutAt < curTime){
      sendASync(_queue[_front], _queue[_front][MAX_PAYLOAD_SIZE - 2], _queue[_front][MAX_PAYLOAD_SIZE - 1]);
    }
    return true;
  }
  return false;  // nothing left to send
}

// instruct the manager to try and send a message from the queue (if there are any)
int ATTDevice::processQueue()
{
  //PRINTLN(_modem->isFree())
  bool sendResult = false;
  if(_modem->isFree() == true)
    return (int)trySendFront();
  else if(_modem->checkSendState(sendResult) == true){
    if(sendResult == true){  // modem succesfully sent a packet, so remove from queue
      PRINTLN("modem reported successfull send")
      _sendFailed = false;
      pop();
      return (int)trySendFront();
    }
    else{
      PRINTLN("modem reported failed send")
      _modem->stop();
      _sendFailed = true;
      return -1;
    }
  }
  return 1;  // there is still work to be done: modem is not free and still working on something
}

// add a message to the queue
bool ATTDevice::addToQueue(void* packet, unsigned char size, bool ack)
{
  // check if the packet is not too big
  if(size > _modem->maxPayloadForSF()){
    PRINTLN("Data size exceeds limitations for current spreading factor.")
    return false;
  }
  short nrRetries = 0;
  unsigned long curTime = millis();
  if(isQueueEmpty() == false ||    // there could be previous payloads waiting to be sent first
     _modem->isFree() == false ||  // or the modem could still be processing a packet
     (_lastTimeSent != 0 && _lastTimeSent + _minTimeBetweenSend > curTime))  // or it's not yet time to send a new packet
  {
    if(isQueueFull() == true){
      PRINTLN("buffer is full, can't transmit packet");
      return false;
    }
    else{
      push(packet, size, ack);
      return true;
    }
  }
  else{
    push(packet, size, ack);
    sendASync(packet, size, ack);
    return true;
  }
}

void ATTDevice::sendASync(void* packet, unsigned char size, bool ack)
{
  // calculate for current settings, before send
  float toa = _modem->calculateTimeOnAir(size);  
  bool canSend = true;     // if the modem doesn't respond to the reconnect, don't try to send
  if(_sendFailed == true)  // restart the modem if a previous send had failed. This connects us back to the base station
    canSend = checkInitStatus();

  if(canSend){
    PRINT("TOA: ") PRINTLN(toa)
    _modem->sendAsync(packet, size, ack);
    _lastTimeSent = millis();
    unsigned long minTime = ceil(toa * 100);  // dynamically adjust
    if(_autoCalMinTime)
      _minTimeBetweenSend = minTime > _minAllowedTimeBetweenSend ? minTime : _minAllowedTimeBetweenSend;
    PRINT("min delay until next send: ") PRINT(_minTimeBetweenSend) PRINTLN(" ms")
  }  
}

void ATTDevice::pop()
{
  if(_front != _back){  // if both are the same there is nothing to pop
    _front++;
    if(_front >= QUEUESIZE)
      _front = 0;
  }
}

void ATTDevice::push(void* data, unsigned char size, bool ack)
{
  /*
  PRINTLN("buffering payload:")
  for (unsigned char i = 0; i < size; i++) {
    PRINT(((unsigned char*)data)[i], HEX) PRINT(" ")
    //_modem->printHex(((unsigned char*)data)[i]);
  }
  PRINTLN();
  */
  
  memcpy(_queue[_back], data, size);
  _queue[_back][MAX_PAYLOAD_SIZE - 2] = size;
  _queue[_back][MAX_PAYLOAD_SIZE - 1] = ack;
  _back++;
  if(_back >= QUEUESIZE)
    _back = 0;
}
