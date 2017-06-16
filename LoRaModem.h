/*
AllThingsTalk - Abstract class for LoRa modems

   Copyright 2015-2016 AllThingsTalk

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.

Original author: Jan Bogaerts (2015-2017)
*/

#ifndef LoRaModem_h
#define LoRaModem_h

#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

#include <instrumentationParamEnum.h>

#define SENDSTATE_TRANSMITCOMMAND 0				//3 states used to track async send status
#define SENDSTATE_EXPECTOK 1
#define SENDSTATE_GETRESPONSE 2
#define SENDSTATE_DONE 3

#define MAX_PAYLOAD_SIZE 222					//maximum allowed size for a payload
#define SMALLEST_PAYLOAD_SIZE 51			 	//the payload size for the biggest sf


//callback signature for functions that process data coming from the NSP that were send to the device
//first param: start of byte array that was found
//second param: the length of the package
#define ATT_CALLBACK_SIGNATURE void (*callback)(const uint8_t*,unsigned int)

/*
 base class for modems.
*/
class LoRaModem
{
	public:
		/** 
		Create the modem object.
		
		parameters:
		- monitor: a stream object, used to write output data towards
		- MQTT_CALLBACK_SIGNATURE: assign a callback function that is called when incoming data (from nsp to device) needs to be processed. Null by default, so no callback will be performed
		*/
		LoRaModem(Stream *monitor, ATT_CALLBACK_SIGNATURE = NULL);
	
		/**
		Returns the required baudrate for the device
		
		returns: an unsigned integer, representing the default baut rate.
		*/
		virtual unsigned int getDefaultBaudRate() = 0;
		
		/**
		Stop the modem.
		
		returns: true upon success.
		*/
		virtual bool Stop() = 0;
		
		/** 
		Set the modem in LoRaWan mode (versus private networks).
		
		parameters:
		- adr: when true, use adaptive data rate (default).
		
		returns: true upon success.
		*/
		virtual bool SetLoRaWan(bool adr = true) = 0;
		
		/**
		Assign a device address to the modem.
		
		parameters:
		- devAddress: the device address to used. Must be 4 bytes long
		
		returns: true upon success.
		*/
		virtual bool SetDevAddress(const unsigned char* devAddress) = 0;
		
		
		/**
		Set the app session key for the modem communication.
		
		parameters:
		- appkey: the app session key, must be 16 bytes long
		
		returns: true upon success.
		*/
		virtual bool SetAppKey(const unsigned char* appKey) = 0;
		
		/** set the network session key
		
		parameters:
		- nwksKey: the network session key, must be 16 bytes long
		
		returns: true upon success.
		*/
		virtual bool SetNWKSKey(const unsigned char*  nwksKey) = 0;
		
		/**
		Start the modem.
		
		returns: true upon success.
		*/
		virtual bool Start() = 0;
		
		/**
		Send a data packet to the NSP.
		
		This operation is performed synchronically, so if ack is requested, then the function will block untill the base station has responded or the time out has expired.
		
		parameters:
		- data: the byte array or pointer to a structure that needs to be sent
		- size: the nr of bytes in the data block
		- ack: when true, an acknowledge is request fromo the base station (default), otherwise no acknowledge is waited for
		
		returns: true upon success.
		*/
		virtual bool Send(void* packet, unsigned char size, bool ack = true);
		
		/**
		Start the send process, but return before everything is done.
		
		This operation is performed asynchronically, so if an ack is requested, then the operation is not yet complete when this function returns.  Consecutive CheckSendState() calls should be performed untill the operation has been completed.
		
		returns: true if the packet was succesfully send, and the process of waiting for a resonse can begin. Otherwise, it returns false.
		*/
		virtual bool SendAsync(void* packet, unsigned char size, bool ack = true) = 0;
		
		/**
		Checks the status of the current send operation (if there was any).
		
		parameters:
		- sendResult:  the result of the send operation, if there was still a pending operation
		 
		returns: if there was none or the operation is done
		*/
		virtual bool CheckSendState(bool& sendResult) = 0;
		
		/**
		returns: true if the modem can send a payload. If it can't at the moment (still processing another packet), then false is returned.
		*/
		inline bool IsFree(){ return sendState == SENDSTATE_DONE; };
		
		/**
		Process any incoming packets from the modem.
		*/
		virtual void ProcessIncoming() = 0;
		
		/**
		Extract the specified instrumentation parameter from the modem and return the value.
		
		You don't normally call this function yourself. Instead use the InstrumentationPacket, which is able to display and send all relative parameter values. This function is also used internally to calculated delays between consecutive send operations.
		
		parameters:
		- param: the id of the parameter whose value should be returned
		
		returns: the value of the specified parameter.
		*/
		virtual int GetParam(instrumentationParam param) = 0;
		
		/**
		returns: the id number of the modem type. See the container definition for the instrumentation container to see more details.
		*/
		virtual int GetModemId() = 0;
		
		/**calcualte the max payload size, based on the current spreading factor of the modem. Used to check if the packet can be sent.
		
		parameters:
		- spreading_factor: the spreading factor to calculate the max payload size for.
		
		returns: the maximum size of the payload for the given spreading factor.
		*/
		int maxPayloadForSF(short spreading_factor = -1);
		
		
		/**
		Calcualte the time on air, based on the current spreading factor and payload size.
		
		parameters:
		- appPayloadSize: the size of the payload to send.
		- spreading_factor: the spreading factor to calculate the max payload size for.
		
		returns: the time on air.
		*/
		float calculateTimeOnAir(unsigned char appPayloadSize, short spreading_factor = -1);
		
		/**
		Get the current state of the (async) send operation.
		*/
		char GetSendState() {return sendState;};
		
	protected:
		Stream *_monitor;
		void (*_callback)(const uint8_t*,unsigned int);
		//keeps track of the current async send position: are we waiting for 'ok' or response.
		char sendState;
		
		//stores the previous payload size. This is for in case that the payload size is requested while we are waiting on a response from
		//the modem. In this case, we can't request the sf from the modem, so we can't calculate the value. Instead we use this buffered val.
		int _prevPayloadForSF;
		
		void printHex(unsigned char hex);
		
		float calculateSymbolTime(short spreading_factor = -1, short bandwidth = -1) ;
		int calculateSymbolsInPayload(unsigned char appPayloadSize, short spreading_factor) ;
	
};

#endif
