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

#include <ATT_IOT_LoRaWAN.h>
#include "Utils.h"
//#include <arduino.h>			// still required for the 'delay' function. use #ifdef for other platforms


//create the object
ATTDevice::ATTDevice(LoRaModem* modem, Stream* monitor, bool autoCalMinTime, unsigned int minTime):  _minTimeBetweenSend(minTime), 
																									 _autoCalMinTime(autoCalMinTime), 
																									 _minAllowedTimeBetweenSend(minTime)
{
  _modem = modem;
  _monitor = monitor;
  _back = _front = _lastTimeSent = 0;
  _sendFailed = false;
}

//connect with the to the lora gateway
bool ATTDevice::InitABP(const uint8_t* devAddress, const uint8_t* appKey, const uint8_t* nwksKey, bool adr)
{
	_devAddress = devAddress;
	_appKey = appKey;
	_nwksKey = nwksKey;
  
  if(!HasKeys())
  {
    PRINTLN("Program stopped");
    while(1);
  }
  
	_adr = adr;
	PRINT("ATT lib version: "); PRINTLN(VERSION);
	if(!_modem->Stop()){  // stop any previously running modems
		PRINTLN("can't communicate with modem: possible hardware issues");
		return false;
	}
	return CheckInitStatus();
}

bool ATTDevice::HasKeys()
{
  // check if keys are filled in
  int sum[3] = {0,0,0};
  for(int i=0; i<sizeof(_devAddress); i++)
  {
    sum[0] += _devAddress[i];
  }
  for(int i=0; i<sizeof(_appKey); i++)
  {
    sum[1] += _appKey[i];
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

bool ATTDevice::CheckInitStatus()
{
	if (!_modem->SetLoRaWan(_adr)){  // switch to LoRaWAN mode instead of peer to peer				
		PRINTLN("can't set adr: possible hardware issues?");
		return false;
	}
	if(!_modem->SetDevAddress(_devAddress)){
		PRINTLN("can't assign device address to modem: possible hardware issues?");
		return false;
	}
	if(!_modem->SetAppKey(_appKey)){
		PRINTLN("can't assign app session key to modem: possible hardware issues?");
		return false;
	}
	if(!_modem->SetNWKSKey(_nwksKey)){
		PRINTLN("can't assign network session key to modem: possible hardware issues?");
		return false;
	}
	bool result = _modem->Start();								//start the modem up 
	if(result == true){
		PRINTLN("modem initialized");
	}
	else{
		PRINTLN("Parameters loaded, but modem won't start: initialization failed");
	}
	return result;									//we have created a connection successfully.
}


bool ATTDevice::trySendFront()
{
	if(IsQueueEmpty() == false){
		unsigned long curTime = millis();
		unsigned long timeoutAt = _lastTimeSent + _minTimeBetweenSend;
		if(timeoutAt < curTime){
			SendASync(_queue[_front], _queue[_front][MAX_PAYLOAD_SIZE - 2], _queue[_front][MAX_PAYLOAD_SIZE - 1]);
		}
		return true;
	}
	return false;  // nothing left to be sent
}

//instructs the manager to try and send a message from it's queue (if there are any)
int ATTDevice::ProcessQueue()
{
	//PRINTLN(_modem->IsFree())
	bool sendResult = false;
	if(_modem->IsFree() == true)
		return (int)trySendFront();
	else if(_modem->CheckSendState(sendResult) == true){
		if(sendResult == true){  // modem succesfully sent a packet, so remove from queue
			PRINTLN("modem reported successfull send")
			_sendFailed = false;
			Pop();
			return (int)trySendFront();
		}
		else{
			PRINTLN("modem reported failed send")
			_modem->Stop();
			_sendFailed = true;
			return -1;
		}
	}
	return 1;  // there is still work to be done: modem is not free and still working on something
}

bool ATTDevice::AddToQueue(void* packet, unsigned char size, bool ack)
{
	//check if the packet is not too big
	if(size > _modem->maxPayloadForSF()){
		PRINTLN("Data size exceeds limitations for current spreading factor.")
		return false;
	}
	short nrRetries = 0;
	unsigned long curTime = millis();
	if(IsQueueEmpty() == false ||    // there could be previous payloads waiting to be sent first
	   _modem->IsFree() == false ||  // or the modem could still be processing a packet.
	   (_lastTimeSent != 0 && _lastTimeSent + _minTimeBetweenSend > curTime))	 // or it's not yet time to send a new packet
	{
		if(IsQueueFull() == true){
			PRINTLN("buffer is full, can't transmit packet");
			return false;
		}
		else{
			Push(packet, size, ack);
			return true;
		}
	}
	else{
		Push(packet, size, ack);  // also need to buffer it, otherwise we do a pop, which is not valid.
		SendASync(packet, size, ack);
		return true;
	}
	
}

void ATTDevice::SendASync(void* packet, unsigned char size, bool ack)
{
	float toa = _modem->calculateTimeOnAir(size);  // calculate for current settings, so BEFORE send !! -> do before checking if the previous send has failed, this way, we don't loose speed after dropping the connection (otherwise delay goes back to 130 sec, which is not required).
	bool canSend = true;     // if the modem doesn't respond to the reconnect, don't try to send.
	if(_sendFailed == true)  // restart the modem if a previous send had failed. This connects us back to the base station.
		canSend = CheckInitStatus();
	// calculate BEFORE or AFTER send ??  (-> sf might change ... before would be the actual value used in send)
	if(canSend){
		PRINT("TOA: ") PRINTLN(toa)
		_modem->SendAsync(packet, size, ack);
		_lastTimeSent = millis();
		unsigned long minTime = ceil(toa * 100);  //dynamically adjust
		if(_autoCalMinTime)
			_minTimeBetweenSend = minTime > _minAllowedTimeBetweenSend ? minTime : _minAllowedTimeBetweenSend;
		PRINT("min delay until next send: ") PRINT(_minTimeBetweenSend) PRINTLN(" ms")
	}	
}


void ATTDevice::Pop()
{
	if(_front != _back){  // if both are the same there is nothing to pop
		_front++;
		if(_front >= QUEUESIZE)
			_front = 0;
	}
}

void ATTDevice::Push(void* data, unsigned char size, bool ack)
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
