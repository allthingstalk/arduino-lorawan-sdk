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

#include "MicrochipLoRaModem.h"
#include "StringLiterals.h"
#include "Utils.h"
#include <Arduino.h>

#define PORT 0x01
#define FULLDEBUG

#define PACKET_TIME_OUT 45000


typedef struct StringEnumPair
{
  const char* stringValue;
  uint8_t enumValue;
} StringEnumPair;

MicrochipLoRaModem::MicrochipLoRaModem(SerialType* stream, Stream *monitor, ATT_CALLBACK_SIGNATURE): LoRaModem(monitor, callback)
{
  _stream = stream;
  _monitor = monitor;
}

bool MicrochipLoRaModem::stop()
{
#ifdef FULLDEBUG
  PRINTLN("[resetDevice]");
#endif

  _stream->print(STR_CMD_RESET);
  _stream->print(CRLF);
  bool result = true;
  if(!expectString(STR_DEVICE_TYPE_EU) && !checkInputInstring(STR_DEVICE_TYPE_US)){
    #ifdef FULLDEBUG
      PRINTLN("Initial reset failed, starting wakeup sequence");
    #endif
    
    // Try to wakeup the modem and send the messages again.
    // Sometimes the modem is just not correctly woken up after a new sketch was loaded
    wakeUp();
    #ifdef FULLDEBUG
      PRINTLN("Retrying reset");
    #endif
    _stream->print(STR_CMD_RESET);
    _stream->print(CRLF);
    result = expectString(STR_DEVICE_TYPE_EU) || checkInputInstring(STR_DEVICE_TYPE_US);
  }
  if(result){
    PRINT("Modem type: "); 
    if (strstr(this->inputBuffer, STR_DEVICE_TYPE_EU) != NULL){
      PRINTLN(STR_DEVICE_TYPE_EU);
      }
    else if (strstr(this->inputBuffer, STR_DEVICE_TYPE_US) != NULL){
      PRINTLN(STR_DEVICE_TYPE_US);
    }
  }
  return result;
}

char MicrochipLoRaModem::checkInputInstring(const char* str)
{
  if (strstr(this->inputBuffer, str) != NULL)
  {
    #ifdef FULLDEBUG
    PRINTLN(" found a match!");
    #endif
    return 1;
  }
  return 0;
}

bool MicrochipLoRaModem::setLoRaWan(bool adr)
{
  // lorawan should be on by default (no private supported)
  return setMacParam(STR_ADR, BOOL_TO_ONOFF(adr));  // set to adaptive variable rate transmission
}

unsigned int MicrochipLoRaModem::getDefaultBaudRate() 
{ 
  return 57600; 
};

bool MicrochipLoRaModem::setDevAddress(const unsigned char* devAddress)
{
#ifdef FULLDEBUG
  PRINTLN("Setting the DevAddr");
#endif  
  return setMacParam(STR_DEV_ADDR, devAddress, 4); 
}

bool MicrochipLoRaModem::setAppsKey(const unsigned char* appsKey)
{
  #ifdef FULLDEBUG
  PRINTLN("Setting the AppSKey"); 
  #endif  
  return setMacParam(STR_APP_SESSION_KEY, appsKey, 16);
}

bool MicrochipLoRaModem::setNWKSKey(const unsigned char*  nwksKey)
{
  #ifdef FULLDEBUG
  PRINTLN("Setting the NwkSKey"); 
  #endif  
  return setMacParam(STR_NETWORK_SESSION_KEY, nwksKey, 16);
}

bool MicrochipLoRaModem::start()
{
  #ifdef FULLDEBUG
  PRINTLN("Sending the network start commands");
  #endif    
  _stream->print(STR_CMD_JOIN);
  _stream->print(STR_ABP);
  _stream->print(CRLF);

  #ifdef FULLDEBUG  
  PRINT(STR_CMD_JOIN);
  PRINT(STR_ABP);
  PRINT(CRLF);
  #endif

  if(expectOK()){
    if(expectString(STR_ACCEPTED))
      return true;
    else
      PRINTLN("Join request rejected")
  }
  
  return false;
}

#ifdef ENABLE_SLEEP
void MicrochipLoRaModem::sleep()
{
  #ifdef FULLDEBUG
  PRINTLN("putting the modem into sleep mode");
  #endif    
  _stream->print(STR_CMD_SLEEP);
  _stream->print(CRLF);

  #ifdef FULLDEBUG  
  PRINT(STR_CMD_SLEEP);
  PRINT(CRLF);
  #endif
}

void MicrochipLoRaModem::wakeUp()
{
  #ifdef FULLDEBUG
	PRINTLN("wakeup the modem");
	#endif
	// "emulate" break condition
  _stream->flush();
  //_stream->end();
  _stream->begin(300);
  _stream->write((uint8_t)0x00);
  _stream->flush();
  //_stream->end();

  delay(50);

  // set baudrate
  _stream->begin(getDefaultBaudRate());
  _stream->write((uint8_t)0x55);
  _stream->flush();

  readLn();
  
}
#endif

// send a data packet to the server
bool MicrochipLoRaModem::sendAsync(void* packet, unsigned char size, bool ack)
{
  if(LoRaModem::send(packet, size, ack)){  // check size, copy to buffer, show hex on debug serial. If this is not ok, don't try to send.
    sendState = SENDSTATE_TRANSMITCOMMAND;
    if(ack == true)
      macSendCommand(STR_CONFIRMED, (unsigned char*)packet, size);
    else
      macSendCommand(STR_UNCONFIRMED, (unsigned char*)packet, size);
    sendState = SENDSTATE_EXPECTOK;
    _triedReadOk = false;  // we start looking for 'ok', we have never tried to read the response yet. This is used to make certain we try to read a response at least 1 time before timing out.      
    asyncOperationStart = millis();  // so we know when the async operation started.
    return true;
  }
  sendState = SENDSTATE_DONE;
  return false;
}

/**
 * Check the status of the current send operation (if there was any)
 * If there was none or the operation is done, then true is returned
 * else the result of the send operation is returned in the param 'sendResult'
 */
bool MicrochipLoRaModem::checkSendState(bool& sendResult)
{
  sendResult = false;
  unsigned long curTime = millis();
  if(sendState == SENDSTATE_EXPECTOK)
  {
    #ifdef FULLDEBUG  
    PRINTLN("cur state: EXPECTOK")
    #endif
    unsigned long timeoutAt = asyncOperationStart + DEFAULT_TIMEOUT;
    if ( (timeoutAt < asyncOperationStart && curTime > timeoutAt && curTime < asyncOperationStart) || (curTime > timeoutAt && _triedReadOk == true )){
      PRINTLN("LoRa: timed-out waiting for a 'ok' response from modem")
      sendState = SENDSTATE_DONE;
      return true;
    }
    else
    {
      #ifdef FULLDEBUG  
      PRINT(".");
      #endif
      _triedReadOk = true;
      char readResult = tryReadString(STR_RESULT_OK);
      #ifdef FULLDEBUG
      PRINT("try read string response: "); PRINTLN((int)readResult);
      #endif
      if (readResult == 0){
        PRINTLN("LoRa: invalid response from modem, expected ok")
        sendState = SENDSTATE_DONE;
        return true;  // the operation failed
      }
      else if(readResult == 1){
        sendState = SENDSTATE_GETRESPONSE;  // go to next stage
        asyncOperationStart = millis();     // new async operation has started, so new timeout  
      }
      else
        return false;  // we remain in the same state, need to process again
    }
  }
  if(sendState == SENDSTATE_GETRESPONSE)
  {
    #ifdef FULLDEBUG  
    PRINTLN("cur state: EXPECTGETRESPONSE")
    #endif
    unsigned long timeoutAt = asyncOperationStart + RECEIVE_TIMEOUT;
    #ifdef FULLDEBUG  
    PRINT("curTime: ") PRINTLN(curTime)
    PRINT("async start: ") PRINTLN(asyncOperationStart)
    PRINT("timeoutAt: ") PRINTLN(timeoutAt)
    #endif
    if ( (timeoutAt < asyncOperationStart && curTime > timeoutAt && curTime < asyncOperationStart) || curTime > timeoutAt)
    {
      PRINTLN("LoRa: timed-out waiting for a response!")
      sendState = SENDSTATE_DONE;
      return true;  // the operation failed, time to give up
    }
    else{
      unsigned char responseVal = macTransmitGetResponse();
      if(responseVal != NoResponse){  // we have found a response (good or bad)
        sendResult = responseVal == NoError;
        sendState = SENDSTATE_DONE;
        return true;
      }
      return false;  // we are not yet done: didn't find a result
    }
  }
  else if(sendState != SENDSTATE_DONE && sendState != SENDSTATE_EXPECTOK)
  {
    PRINTLN("LoRa internal error: unexpected state, resetting to init");
    sendState = SENDSTATE_DONE;
  }
  return true;  //nothing to wait for
}

//send command
void MicrochipLoRaModem::macSendCommand(const char* type, const unsigned char* payload, unsigned char size)
{
  _stream->print(STR_CMD_MAC_TX);
  _stream->print(type);
  _stream->print(PORT);
  _stream->print(" ");
  
  #ifdef FULLDEBUG  
  PRINT(STR_CMD_MAC_TX);
  PRINT(type);
  PRINT(PORT);
  PRINT(" ");
  #endif

  for (int i = 0; i < size; ++i)
  {
    _stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(payload[i]))));
    _stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(payload[i]))));
    #ifdef FULLDEBUG    
    PRINT(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(payload[i]))));
    PRINT(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(payload[i]))));
    #endif
  }
  _stream->print(CRLF);

  #ifdef FULLDEBUG  
  PRINT(CRLF);
  #endif
}

unsigned char MicrochipLoRaModem::macTransmit(const char* type, const unsigned char* payload, unsigned char size)
{
  macSendCommand(type, payload, size);

  if (!expectOK())
    return TransmissionFailure;

  #ifdef FULLDEBUG
  PRINTLN("Waiting for server response");
  #endif
  unsigned long timeout = millis() + RECEIVE_TIMEOUT;
  while (millis() < timeout)
  {
    #ifdef FULLDEBUG  
    PRINT(".");
    #endif
    unsigned char responseVal = macTransmitGetResponse();
    if(responseVal != NoResponse)  // if we get something other then no response, then we found something, so exit
      return responseVal;
  }
  PRINTLN("Timed-out waiting for a response!");
  return Timeout;
}

unsigned char MicrochipLoRaModem::macTransmitGetResponse()
{
  if (readLn() > 0)
  {
    #ifdef FULLDEBUG    
    PRINT(".(");
    PRINT(this->inputBuffer);
    PRINT(")");
    #endif

    if (strstr(this->inputBuffer, " ") != NULL)  // to avoid double delimiter search
    {
      // there is a splittable line -only case known is mac_rx
      #ifdef FULLDEBUG        
      PRINTLN("Splittable response found");
      #endif
      onMacRX();
      return NoError;
    }
    else if (strstr(this->inputBuffer, STR_RESULT_MAC_TX_OK))
    {
      // done
      #ifdef FULLDEBUG
      PRINTLN("Received mac_tx_ok");
      #endif
      return NoError;
    }
    else
    {
      #ifdef FULLDEBUG
      PRINTLN("Some other string received (error)");
      #endif
      return lookupMacTransmitError(this->inputBuffer);
    }
  }
  return NoResponse;
}

uint8_t MicrochipLoRaModem::lookupMacTransmitError(const char* error)
{
  PRINT("[lookupMacTransmitError]: ");
  PRINTLN(error);

  if (error[0] == 0)
  {
    PRINTLN("[lookupMacTransmitError]: The string is empty!");
    return NoResponse;
  }

  StringEnumPair errorTable[] =
  {
    { STR_RESULT_OK, NoError },
    { STR_RESULT_INVALID_PARAM, TransmissionFailure },
    { STR_RESULT_NOT_JOINED, TransmissionFailure },
    { STR_RESULT_NO_FREE_CHANNEL, TransmissionFailure },
    { STR_RESULT_SILENT, TransmissionFailure },
    { STR_RESULT_FRAME_COUNTER_ERROR, TransmissionFailure },
    { STR_RESULT_BUSY, TransmissionFailure },
    { STR_RESULT_MAC_PAUSED, TransmissionFailure },
    { STR_RESULT_INVALID_DATA_LEN, TransmissionFailure },
    { STR_RESULT_MAC_ERROR, TransmissionFailure },
    { STR_RESULT_MAC_TX_OK, NoError }
  };

  for (StringEnumPair* p = errorTable; p->stringValue != NULL; ++p) 
  {
    if (strcmp(p->stringValue, error) == 0)
    {
      PRINT("[lookupMacTransmitError]: found ");
      PRINTLN(p->enumValue);

      return p->enumValue;
    }
  }

  PRINTLN("[lookupMacTransmitError]: not found!");
  return NoResponse;
}

// waits for string, if str is found returns ok, if other string is found returns false, if timeout returns false
bool MicrochipLoRaModem::expectString(const char* str, unsigned short timeout)
{
  #ifdef FULLDEBUG
  PRINT("[expectString] expecting ");
  PRINTLN(str);
  #endif

  unsigned long start = millis();
  while (millis() < start + timeout)
  {
    #ifdef FULLDEBUG  
    PRINT(".");
    #endif
    char readResult = tryReadString(str);
    if (readResult == 0) return false;
    else if(readResult == 1) return true;
  }
  return false;
}

/**
 * Try to read a string from the input, 1 time
 *
 * @return
 * -1: nothing on the buffer, try again next time
 *  0: found something that didn't match
 *  1: found something that matched
 */
char MicrochipLoRaModem::tryReadString(const char* str)
{
  if (readLn() > 0)
  {
    #ifdef FULLDEBUG    
    PRINT("(");
    PRINT(this->inputBuffer);
    PRINT(")");
    #endif

    return checkInputInstring(str);
  }
  return -1;
}

/**
 * Try to read a string from the input, until it is found or until the timeout occured. This is done async
 *
 * @return
 * -1: nothing on the buffer, try again next time
 *  0: found something that didn't match
 *  1: found something that matched
 */
char expectStringAsync(const char* str, unsigned short timeout)
{

}

unsigned short MicrochipLoRaModem::readLn(char* buffer, unsigned short size, unsigned short start)
{
  int len = _stream->readBytesUntil('\n', buffer + start, size);
  if(len > 0)
    buffer[start + len - 1] = 0;  // bytes until \n always end with \r, so get rid of it (-1)
  else
    buffer[start] = 0;

  return len;
}

bool MicrochipLoRaModem::expectOK()
{
  bool res = expectString(STR_RESULT_OK);
  if (!res)  // print an error message that describes the result for the user
  {
    if (strstr(this->inputBuffer, "invalid_param") != NULL)
      PRINTLN("An invalid parameter was sent to the modem")
    else if (strstr(this->inputBuffer, "keys_not_init") != NULL)
      PRINTLN("The lora keys corresponding to the join mode (otaa or abp) were not properly configured")
    else if (strstr(this->inputBuffer, "no_free_ch") != NULL)
      PRINTLN("All LoRa channels are currently busy")
    else if (strstr(this->inputBuffer, "silent") != NULL)
      PRINTLN("The device is in silent Immediately mode")
    else if (strstr(this->inputBuffer, "busy") != NULL)
      PRINTLN("The MAC state is not in an idle state")
    else if (strstr(this->inputBuffer, "mac_paused") != NULL)
      PRINTLN("The MAC has been paused and not resumed back")
    else if (strstr(this->inputBuffer, "not_joined") != NULL)
      PRINTLN("The network is not joined")
    else if (strstr(this->inputBuffer, "frame_counter_err_rejoin_needed") != NULL)
      PRINTLN("The frame counter rolled over, possible rejoin required")
    else if (strstr(this->inputBuffer, "invalid_data_len") != NULL)
      PRINTLN("The application payload length is greater than the maximum application payload length corresponding to the current data rate")
  }
}

// paramName should include the trailing space
bool MicrochipLoRaModem::setMacParam(const char* paramName, const unsigned char* paramValue, unsigned short size)
{
  #ifdef FULLDEBUG
  PRINT("[setMacParam] ");
  PRINT(paramName);
  PRINT("= [array]");
  #endif

  _stream->print(STR_CMD_SET);
  _stream->print(paramName);
  #ifdef FULLDEBUG
  PRINT(STR_CMD_SET);
  PRINT(paramName);
  #endif

  for (unsigned short i = 0; i < size; ++i)
  {
    _stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))));
    _stream->print(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i]))));
    #ifdef FULLDEBUG
    PRINT(static_cast<char>(NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(paramValue[i]))));
    PRINT(static_cast<char>(NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(paramValue[i])))); 
    #endif
  }
  
  _stream->print(CRLF);
  #ifdef FULLDEBUG
  PRINT(CRLF);
  #endif

  return expectOK();
}

// paramName should include the trailing space
bool MicrochipLoRaModem::setMacParam(const char* paramName, uint8_t paramValue)
{
  #ifdef FULLDEBUG
  PRINT("[setMacParam] ");
  PRINT(paramName);
  PRINT("= ");
  PRINTLN(paramValue);
  #endif

  _stream->print(STR_CMD_SET);
  _stream->print(paramName);
  _stream->print(paramValue);
  _stream->print(CRLF);
  
  #ifdef FULLDEBUG
  PRINT(STR_CMD_SET);
  PRINT(paramName);
  PRINT(paramValue);
  PRINT(CRLF);
  #endif

  return expectOK();
}

// paramName should include the trailing space
bool MicrochipLoRaModem::setMacParam(const char* paramName, const char* paramValue)
{
  _stream->print(STR_CMD_SET);
  _stream->print(paramName);
  _stream->print(paramValue);
  _stream->print(CRLF);
  
  #ifdef FULLDEBUG
  PRINT(STR_CMD_SET);
  PRINT(paramName);
  PRINT(paramValue);
  PRINT(CRLF);
  #endif
  
  return expectOK();
}

// process any incoming packets from the modem
void MicrochipLoRaModem::processIncoming()
{
  readLn();
}

unsigned char MicrochipLoRaModem::onMacRX()
{
  if(_callback != NULL){
    // parse inputbuffer, put payload into packet buffer
    char* token = strtok(this->inputBuffer, " ");

    // sanity check
    if (strcmp(token, STR_RESULT_MAC_RX) != 0){
      PRINTLN("no mac_rx found in result");
      return NoResponse; // TODO create more appropriate error codes
    }
    token = strtok(NULL, " ");  // port
    token = strtok(NULL, " ");  //payload, until end of string 
    
    unsigned char buffer[(strlen(token) / 2) + 1];  // each bytes is represented by 2 chars, so /2 for the length
    uint16_t outputIndex = 0;
    
    // stop at the first string termination char, or if output buffer is over, or if payload buffer is over
    while (outputIndex < DEFAULT_RECEIVED_PAYLOAD_BUFFER_SIZE && *token != 0 && *(token + 1) != 0) {  // token must end at 0, otherwise it's no string, cant find end
      buffer[outputIndex] = HEX_PAIR_TO_BYTE(*token, *(token + 1));
      token += 2;
      outputIndex++;
    }
    buffer[outputIndex] = 0;  // make certain we close it if it's a string
    _callback(buffer, outputIndex);
  }
  return NoError;
}

// extract the specified instrumentation parameter from the modem and return the value
int MicrochipLoRaModem::getParam(instrumentationParam param)
{
  switch(param){
    case MODEM: return 0;
    case FREQUENCYBAND:{
      if (strstr(getMacParam("band"), "868")) return 1;
      return 0;}
    case SP_FACTOR:{
      char* val = getRadioParam("sf");
      return sfToIndex(val);}
    case ADR:{
      if (strstr(getMacParam("adr"), "on")) return 1;
      return 0;}
    case POWER_INDEX:{
      char* val = getMacParam("pwridx");
      return atoi(val);}
    case BANDWIDTH:{
      char* val = getRadioParam("bw");
      if (strstr(val, "500")) return 3;
      if (strstr(val, "250")) return 2;
      if (strstr(val, "125")) return 1;
      return 0;}
    case CODING_RATE:{
      char* val = getRadioParam("cr");
      if (strstr(val, "4/8")) return 3;
      if (strstr(val, "4/7")) return 2;
      if (strstr(val, "4/6")) return 1;
      return 0;}
    case DUTY_CYCLE:{
      char* val = getMacParam("dcycleps");
      return atoi(val);}
    case SNR:{
      char* val = getRadioParam("snr");
      return atoi(val);}
    case GATEWAY_COUNT:{
      char* val = getMacParam("gwnb");
      return atoi(val);}
    case RETRANSMISSION_COUNT:{
      char* val = getMacParam("retx");
      return atoi(val);}
    case DATA_RATE:{
      char* val = getMacParam("dr");
      return atoi(val);}
    default: return false;
  }
}

char* MicrochipLoRaModem::getSysParam(const char* paramName, unsigned short timeout)
{
  #ifdef FULLDEBUG
  PRINT("[getMacParam] ");
  PRINT(paramName);
  #endif

  _stream->print(STR_CMD_GET_SYS);
  _stream->print(paramName);
  _stream->print(CRLF);

  unsigned long start = millis();
  while (millis() < start + timeout)
  {
    if (readLn() > 0)
    {
      #ifdef FULLDEBUG
      PRINT(" = ");
      PRINTLN(this->inputBuffer);
      #endif
      return this->inputBuffer;
    }
  }
  return NULL;  // no repsonse, so return empty string
}

char* MicrochipLoRaModem::getMacParam(const char* paramName, unsigned short timeout)
{
  #ifdef FULLDEBUG  
  PRINT("[getMacParam] ");
  PRINT(paramName);
  #endif

  _stream->print(STR_CMD_GET_MAC);
  _stream->print(paramName);  
  _stream->print(CRLF);
  
  unsigned long start = millis();
  while (millis() < start + timeout)
  {
    if (readLn() > 0)
    {
      #ifdef FULLDEBUG
      PRINT(" = ");
      PRINTLN(this->inputBuffer);
      #endif
      return this->inputBuffer;
    }
  }
  return NULL;  // no repsonse, so return empty string
}

char* MicrochipLoRaModem::getRadioParam(const char* paramName, unsigned short timeout)
{
  #ifdef FULLDEBUG  
  PRINT("[getRadioParam] ");
  PRINT(paramName);
  #endif

  _stream->print(STR_CMD_GET_RADIO);
  _stream->print(paramName);  
  _stream->print(CRLF);
  
  unsigned long start = millis();
  while (millis() < start + timeout)
  {
    if (readLn() > 0)
    {
      #ifdef FULLDEBUG
      PRINT(" = ");
      PRINTLN(this->inputBuffer);
      #endif
      return this->inputBuffer;
    }
  }
  return NULL;  // no repsonse, so return empty string
}

// convert the text value for spreading factor into a number between 0 and 6
int MicrochipLoRaModem::sfToIndex(char* value)
{
  int len = strlen(value);
  if(len == 3){
    int res = value[2] - 54;              // 48 = ascii 0, ascii 55 = 7 -> translates to index 0x01
    if (res >= 1 && res < 4) return res;  // small sanity check, make certain that it is within the expected range
  }
  else if(len == 4){
    int res = value[3] - 47 + 3;
    if (res >= 4 && res < 7) return res;  // small sanity check, make certain that it is within the expected range
  }
  return 0;  // unknown spreading factor
}

// returns the id number of the modem type. See the container definition for the instrumentation container to see more details
int MicrochipLoRaModem::getModemId()
{
  return 3;
}