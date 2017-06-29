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

#include <LoRaModem.h>
#include <Utils.h>

#define LORA_HEADER_SIZE 13
#define EXPLICIT_HEADER true
#define PREAMBLE_SYMBOLS 8

LoRaModem::LoRaModem(Stream *monitor, ATT_CALLBACK_SIGNATURE)
{
	_monitor = monitor;
	_callback = callback;
	sendState = SENDSTATE_DONE;								//init to done state, we are ready to send a new packet.
	_prevPayloadForSF = SMALLEST_PAYLOAD_SIZE;
}

int LoRaModem::maxPayloadForSF(short spreading_factor)  
{	
	if (spreading_factor < 0){
		if(sendState == SENDSTATE_DONE)
			spreading_factor = GetParam(SP_FACTOR) + 6;
		else
			return _prevPayloadForSF;
	}
	if (spreading_factor >= 10)
		_prevPayloadForSF = SMALLEST_PAYLOAD_SIZE;
	else if (spreading_factor < 9)
		_prevPayloadForSF = MAX_PAYLOAD_SIZE;
	else
		_prevPayloadForSF = 115;
	return _prevPayloadForSF;
}

float LoRaModem::calculateSymbolTime(short spreading_factor, short bandwidth) 
{
	if (spreading_factor < 0)
		spreading_factor = GetParam(SP_FACTOR) + 6;
	if (bandwidth < 0)
		bandwidth = (pow(2, GetParam(BANDWIDTH)) - 1) * 125;
	return (pow(2, (float) spreading_factor)) / ((float) bandwidth * 1000.0) * 1000.0;
}

float LoRaModem::calculateTimeOnAir(unsigned char appPayloadSize, short spreading_factor) 
{
	float symbTime = calculateSymbolTime(spreading_factor);
	float symbolsInPayload = (float) calculateSymbolsInPayload(appPayloadSize, spreading_factor);
	return (symbTime * symbolsInPayload) + (symbTime * (4.25 + PREAMBLE_SYMBOLS));
}

int LoRaModem::calculateSymbolsInPayload(unsigned char appPayloadSize, short spreading_factor) 
{
	if (spreading_factor < 0)
		spreading_factor = GetParam(SP_FACTOR) + 6;
	int bandwidth = (pow(2, GetParam(BANDWIDTH)) - 1) * 125;
	int coding_rate = GetParam(CODING_RATE) + 5;
	int low_dr_corr = ((spreading_factor == 11 || spreading_factor == 12) && bandwidth == 125) ? 2 : 0;
	float temp = (((float )(8 * (appPayloadSize + LORA_HEADER_SIZE))
									- (4 * spreading_factor) + 28 + 16
									- (EXPLICIT_HEADER ? 0 : 20))
									/ (float ) (4 * (spreading_factor - low_dr_corr)));
	return 8 + max( ceil(temp) * coding_rate, 0);

}

bool LoRaModem::Send(void* packet, unsigned char size, bool ack)
{
	if(sendState != SENDSTATE_DONE){
		PRINTLN("A previous async send operation is still. Please wait untill done");
		return false;
	}
	if(size > MAX_PAYLOAD_SIZE){
		PRINT("payload too big, can't send packets bigger then "); PRINTLN(MAX_PAYLOAD_SIZE);
		return false;
	}
	PRINTLN("Sending payload: ");
	for (unsigned char i = 0; i < size; i++) {
		printHex(((unsigned char*)packet)[i]);
	}
	PRINTLN();
	return true;
}

void LoRaModem::printHex(unsigned char hex)
{
  char hexTable[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
  PRINT(hexTable[hex /16]);
  PRINT(hexTable[hex % 16]);
  PRINT(' ');
}