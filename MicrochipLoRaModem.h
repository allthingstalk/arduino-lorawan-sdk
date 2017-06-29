/*
  Copyright 2015-2017 AllThingsTalk

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  
  AllThingsTalk Communication with Embit lora modems through binary AT commands
  Original author: Jan Bogaerts
*/

#ifndef EmbitLoRaModem_h
#define EmbitLoRaModem_h

#include <LoRaModem.h>
//#include <LoRaPacket.h>
#include <instrumentationParamEnum.h>
#if defined(ARDUINO) && ARDUINO >= 100
  #include "arduino.h"
#else
  #include "WProgram.h"
#endif

#define DEFAULT_PAYLOAD_SIZE MAX_PAYLOAD_SIZE
#define PORT 1
#define DEFAULT_INPUT_BUFFER_SIZE 440  // needs to be big enough to store 220 hex byte values.
#define DEFAULT_RECEIVED_PAYLOAD_BUFFER_SIZE MAX_PAYLOAD_SIZE
#define DEFAULT_TIMEOUT 120
#define RECEIVE_TIMEOUT 60000
#define MAX_SEND_RETRIES 10


#if defined(ARDUINO_ARCH_AVR)
typedef HardwareSerial SerialType;
#elif defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD)
typedef Uart SerialType;
#else
typedef Stream SerialType;
#endif

#define ENABLE_SLEEP

enum MacTransmitErrorCodes
{
	NoError = 0,
	NoResponse = 1,
	Timeout = 2,
	TransmissionFailure = 3
};

///this class represents the ATT cloud platform.
class MicrochipLoRaModem: public LoRaModem
{
	public:
		/** create the modem object
		
		parameters:
		- stream: the stream object to communicate with the modem over.
		- monitor: a stream object, used to write output data towards.
		- MQTT_CALLBACK_SIGNATURE: assign a callback function that is called when incoming data (from nsp to device) needs to be processed
		  Null by default, so no callback will be performed.
		*/
		MicrochipLoRaModem(SerialType* stream, Stream* monitor = NULL, ATT_CALLBACK_SIGNATURE = NULL);
		
		
		/** Returns the required baudrate for the device
		
		returns: an unsigned integer, representing the default baut rate.
		*/
		unsigned int getDefaultBaudRate();

		/** stop the modem.
		
		returns: true upon success.
		*/
		bool Stop();
		
		/** set the modem in LoRaWan mode (vs private networks)
		
		parameters:
		- adr: when true, use adaptive data rate (default).
		
		returns: true upon success.
		*/
		bool SetLoRaWan(bool adr = true);
		
		/** assign a device address to the modem
		
		parameters:
		- devAddress: the device address to used. Must be 4 bytes long
		
		returns: true upon success.
		*/
		bool SetDevAddress(const unsigned char* devAddress);
		
		/** set the app session key for the modem communication
		
		parameters:
		- appkey: the app session key, must be 16 bytes long
		
		returns: true upon success.
		*/
		bool SetAppKey(const unsigned char* appKey);
		
		/** set the network session key
		
		parameters:
		- nwksKey: the network session key, must be 16 bytes long
		
		returns: true upon success.
		*/
		bool SetNWKSKey(const unsigned char*  nwksKey);
		
		/** start the modem
		
		returns: true upon success.
		*/
		bool Start();
		
		/** start the send process, but return before everything is done.
		
		This operation is performed asynchronically, so if an ack is requested, then the operation is not yet complete when this function
		returns.  Consecutive ChecSendState() calls should be performed untill the operation has been completed.
		
		returns: true if the packet was succesfully send, and the process of waiting for a resonse can begin. Otherwise, it returns false
		*/
		bool SendAsync(void* packet, unsigned char size, bool ack = true);
		
		/** checks the status of the current send operation (if there was any).
		
		parameters:
		- sendResult:  the result of the send operation, if there was still a pending operation.
		 
		 returns: true if there was no pending send operation or the operation is done
		*/
		bool CheckSendState(bool& sendResult);
		
		/** process any incoming packets from the modem
		*/
		void ProcessIncoming();
		
		/** extract the specified instrumentation parameter from the modem and return the value.
		
		You don't normally call this function yourself. Instead use the InstrumentationPacket instead, which is able to display and send
		all relative parameter values.
		This function is also used internally to calculated delays between consecutive send operations.
		
		parameters:
		- param: the id of the parameter whose value should be returned.
		
		returns: the value of the specified parameter.
		*/
		int GetParam(instrumentationParam param);
		
		
		/**returns the id number of the modem type. See the container definition for the instrumentation container to see more details.
		*/
		int GetModemId();
		
		#ifdef ENABLE_SLEEP
		/**put the modem in sleep mode for 3 days (use WakeUp if you want to send something earlier)
		*/
		void Sleep();
		
		/**wakes up the device after it has been put the sleep.
		*/
		void WakeUp();
    
    // retrieves the specified parameter from the MicrochipLoRaModem
    char* getSysParam(const char* paramName, unsigned short timeout = DEFAULT_TIMEOUT);    
    // retrieves the specified parameter from the radio
    char* getRadioParam(const char* paramName, unsigned short timeout = DEFAULT_TIMEOUT);
    // retrieves the specified parameter from the radio
    char* getMacParam(const char* paramName, unsigned short timeout = DEFAULT_TIMEOUT);
    
		#endif
	private:
		Stream *_monitor;
		SerialType* _stream;					//the stream to communicate with the lora modem.
		char inputBuffer[DEFAULT_INPUT_BUFFER_SIZE + 1];
		
		// used to make certain that we at least try to read the modem response for 'ok' 1 time before timing out.
		bool _triedReadOk;
		
		// stores the starting time of the current async operation
		unsigned long asyncOperationStart;
		
		unsigned char lookupMacTransmitError(const char* error);
		unsigned char onMacRX();
		unsigned short readLn(char* buf, unsigned short bufferSize, unsigned short start = 0);
		unsigned short readLn() { return readLn(this->inputBuffer, DEFAULT_INPUT_BUFFER_SIZE); };
		bool expectOK();
		// block until the string is found on input or timeout occured.
		bool expectString(const char* str, unsigned short timeout = DEFAULT_TIMEOUT);
		// try to read the string on the input 1 time.
		char tryReadString(const char* str);
		// try to read the string from teh input until found or untill timeout, do this async, so call many times
		char expectStringAsync(const char* str, unsigned short timeout);
		
		bool setMacParam(const char* paramName, const unsigned char* paramValue, unsigned short size);
		bool setMacParam(const char* paramName, unsigned char paramValue);
		bool setMacParam(const char* paramName, const char* paramValue);
		// send and wait for response
		unsigned char macTransmit(const char* type, const unsigned char* payload, unsigned char size);
		// send command
		void macSendCommand(const char* type, const unsigned char* payload, unsigned char size);
		// convert the text value for spreading factor into a number between 0 and 6
		int sfToIndex(char* value);
		
		unsigned char macTransmitGetResponse();
		// checks the current input string against the param. Returns true if they match.
		char checkInputInstring(const char* str);
};

#endif