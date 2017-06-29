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

#include "Container.h"

// create the object
Container::Container(ATTDevice &device):ContainerPacket(device)
{
}

unsigned char Container::Write(unsigned char* result)
{
	unsigned char curPos =  ContainerPacket::Write(result);

	result[curPos] = boolValues;
	curPos++;
	if(nrInts > 0)
	{
		result[curPos++] = nrInts;
		for(int i = 0; i < nrInts; i++)
		{
			short len = sizeof intValues[i];
			memcpy(result + curPos, &intValues[i], len);
			curPos += len;
		}
	}
	else if(nrFloats > 0 || stringPos > 0)
		result[curPos++] = 0;
	if(nrFloats > 0)
	{
		result[curPos++] = nrFloats;
		for(int i = 0; i < nrFloats; i++)
		{
			short len = sizeof floatValues[i];
			memcpy(result + curPos, &floatValues[i], len);
			curPos += len;
		}
	}
	else if(stringPos > 0)
		result[curPos++] = 0;
	if(stringPos > 0)
	{
		memcpy(result + curPos, stringValues, stringPos);
		curPos += stringPos;
	}
	short packetLen = curPos - 3;  // write the packet length
	memcpy(result + 1, static_cast<const char*>(static_cast<const void*>(&packetLen)), sizeof(short));

	result[curPos] = calculateCheckSum(result + 3, curPos - 3);  // add the checksum
	curPos++;
	return curPos;
}

bool Container::AddToQueue(bool value, short id, bool ack)
{
	unsigned char buffer[220];
	Reset();

	SetId(id);
	Add(value);

	unsigned char length = Write(buffer);
	return _device->AddToQueue(buffer, length, ack);
}

bool Container::Add(bool value)
{
	if(nrBools >= 8)
		return false;
	unsigned char val = value == true ? 1:0;

	val = val << nrBools;
	nrBools++;
	boolValues |= val;
	return true;
}

bool Container::AddToQueue(short value, short id, bool ack)
{
	unsigned char buffer[220];
	Reset();

	SetId(id);
	Add(value);

	unsigned char length = Write(buffer);
	return _device->AddToQueue(buffer, length, ack);
}

bool Container::Add(short value)
{
	if(nrInts >= 16)
		return false;
	intValues[nrInts++] = value;
	return true;
}

bool Container::AddToQueue(String value, short id, bool ack)
{
	unsigned char buffer[220];
	Reset();

	SetId(id);
	Add(value);

	unsigned char length = Write(buffer);
	return _device->AddToQueue(buffer, length, ack);
}

bool Container::Add(String value)
{
	int len = value.length();
	if(stringPos + len >= 48)
		return false;
	value.toCharArray(stringValues + stringPos, len);
	stringPos += len;
	return true;
}

bool Container::AddToQueue(float value, short id, bool ack)
{
	unsigned char buffer[220];
	Reset();

	SetId(id);
	Add(value);

	unsigned char length = Write(buffer);
	return _device->AddToQueue(buffer, length, ack);
}

// send data value to the cloud server for the sensor with the specified id. (x, y, z values)
// if ack = true -> request acknowledge, otherwise no acknowledge is waited for
bool Container::AddToQueue(float x, float y, float z, short id, bool ack)
{
	unsigned char buffer[220];
	Reset();

	SetId(id);
	Add(x);
	Add(y);
	Add(z);

	unsigned char length = Write(buffer);
	return _device->AddToQueue(buffer, length, ack);
}

bool Container::AddToQueue(float x, float y, float z, float time, short id, bool ack)
{
	unsigned char buffer[220];
	Reset();

	SetId(id);
	Add(x);
	Add(y);
	Add(z);
	Add(time);

	unsigned char length = Write(buffer);
	return _device->AddToQueue(buffer, length, ack);
}

bool Container::Add(float value)
{
	if(nrFloats >= 16)
		return false;
	floatValues[nrFloats++] = value;
	return true;
}

void Container::Reset()
{
	stringPos = 0;
	nrFloats = 0;
	nrInts = 0;
	nrBools = 0;
	boolValues = 0;
}

unsigned char Container::GetDataSize() {
	unsigned char cnt = 6;	// LoRaPacket header size

	if (nrInts > 0) {
		cnt += 1;
		cnt += nrInts * sizeof intValues[0];
	} else if (nrFloats > 0 || stringPos > 0) {
		cnt += 1;
	}
	if (nrFloats > 0) {
		cnt += 1;
		cnt += nrFloats * sizeof floatValues[0];
	} else if (stringPos > 0) {
		cnt += 1;
	}
	cnt += stringPos;
	cnt += 1; // crc
	return cnt;
}

const uint8_t* Container::Parse(const uint8_t* data, short& id)
{
    id = 0;
    if(data){
        id = data[0];
        data++;
        return data;
    }
    return NULL;
}
