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

#include "ContainerPacket.h"

// create the object
ContainerPacket::ContainerPacket(ATTDevice &device)
{
	_device = &device;
}

unsigned char ContainerPacket::write(unsigned char* result)
{
	result[0] = 0x7E;
	result[3] = getFrameType();
	result[4] = contId;
	return 5;
}

// assigns the asset/container id to the packet
void ContainerPacket::setId(unsigned char id)
{
	contId = id;
}

unsigned char ContainerPacket::getFrameType()
{
	return 0x40;  // the default packet type
}

unsigned char ContainerPacket::calculateCheckSum(unsigned char* toSend, short len)
{
	int sum = 0;
	for(int i = 0; i < len; i++)
		sum += toSend[i];

	while(sum > 0xFF)
	{
		toSend = (unsigned char*)&sum;
		int newsum = 0;
		len = sizeof(int);
		for(int i = 0; i < len; i++)
			newsum += toSend[i];
		sum = newsum;
	}

	return 0xFF - (unsigned char)sum;
}