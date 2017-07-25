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

#ifndef EmbitLoRaModem_h
#define EmbitLoRaModem_h

#include <LoRaModem.h>
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

// this class represents the ATT cloud platform
class MicrochipLoRaModem: public LoRaModem
{
  public:
    /**
    * Create the modem object
    *
    * @param stream the stream object to communicate with the modem over
    * @param monitor a stream object, used to write output data towards
    * @param MQTT_CALLBACK_SIGNATURE assign a callback function that is called when incoming data (from nsp to device) needs to be processed. Null by default, so no callback will be performed
    */
    MicrochipLoRaModem(SerialType* stream, Stream* monitor = NULL, ATT_CALLBACK_SIGNATURE = NULL);

    /**
     * Returns the required baudrate for the device
     *
     * @return an unsigned integer, representing the default baut rate
     */
    unsigned int getDefaultBaudRate();

    /**
     * Stop the modem
     *
     * @return true upon success
     */
    bool stop();
    
    /**
     * Set the modem in LoRaWan mode (vs private networks)
     *
     * @param adr when true, use adaptive data rate (default)
     *
     * @return true upon success
     */
    bool setLoRaWan(bool adr = true);
    
    /**
     * Assign a device address to the modem
     *
     * @param devAddress the device address to used. Must be 4 bytes long
     *
     * @return true upon success
     */
    bool setDevAddress(const unsigned char* devAddress);
    
    /**
     * Set the app session key for the modem communication
     *  
     * @param appskey the app session key, must be 16 bytes long
     *
     * @return true upon success
     */
    bool setAppsKey(const unsigned char* appsKey);
    
    /**
     * Set the network session key
     *
     * @param nwksKey the network session key, must be 16 bytes long
     *
     * @return true upon success
     */
    bool setNWKSKey(const unsigned char*  nwksKey);
    
    /**
     * Start the modem
     *
     * @return true upon success
     */
    bool start();
    
    /**
     * Start the send process, but return before everything is done.
     * This operation is performed asynchronically, so if an ack is requested,
     * then the operation is not yet complete when this function returns.
     * Consecutive ChecSendState() calls should be performed untill the
     * operation has been completed.
     *
     * @return true if the packet was succesfully send, and the process of waiting for a resonse can begin. Otherwise return false
     */
    bool sendAsync(void* packet, unsigned char size, bool ack = true);
    
    /**
     * Check the status of the current send operation (if there was any)
     *
     * @param sendResult the result of the send operation, if there was still a pending operation
     *
     * @return true if there was no pending send operation or the operation is done
     */
    bool checkSendState(bool& sendResult);
    
    /**
     * Process any incoming packets from the modem
     */
    void processIncoming();
    
    /**
     * Extract the specified instrumentation parameter from the modem and return the value
     *
     * @param param the id of the parameter whose value should be returned
     *
     * @return the value of the specified parameter
     */
    int getParam(instrumentationParam param);
        
    /**
     * Returns the id number of the modem type.
     * See the container definition for the instrumentation container to see more details.
     */
    int getModemId();
    
    #ifdef ENABLE_SLEEP
    /**
     * Put the modem in sleep mode for 3 days (use WakeUp if you want to send something earlier)
     */
    void sleep();
    
    /**
     * Wake up the device after it has been put the sleep
     */
    void wakeUp();
    
    /**
     * Retrieve the specified parameter from the MicrochipLoRaModem
     */
    char* getSysParam(const char* paramName, unsigned short timeout = DEFAULT_TIMEOUT);    
    
    /**
     * Retrieve the specified parameter from the radio
     */
    char* getRadioParam(const char* paramName, unsigned short timeout = DEFAULT_TIMEOUT);
    
    /**
     * Retrieve the specified parameter from the radio
     */
    char* getMacParam(const char* paramName, unsigned short timeout = DEFAULT_TIMEOUT);
    
    #endif
    
  private:
    Stream *_monitor;
    SerialType* _stream;  // the stream to communicate with the lora modem
    char inputBuffer[DEFAULT_INPUT_BUFFER_SIZE + 1];
    
    /**
     * Make certain that we at least try to read the modem response for 'ok' 1 time before timing out
     */
    bool _triedReadOk;
    
    /**
     * Store the starting time of the current async operation
     */
    unsigned long asyncOperationStart;
    
    unsigned char lookupMacTransmitError(const char* error);
    unsigned char onMacRX();
    unsigned short readLn(char* buf, unsigned short bufferSize, unsigned short start = 0);
    unsigned short readLn() { return readLn(this->inputBuffer, DEFAULT_INPUT_BUFFER_SIZE); };
    bool expectOK();

    /**
     * Block until the string is found on input or timeout occured
     */
    bool expectString(const char* str, unsigned short timeout = DEFAULT_TIMEOUT);

    /**
     * Try to read the string on the input 1 time
     */
    char tryReadString(const char* str);
    
    /**
     * Try to read the string from teh input until found or untill timeout, do this async, so call many times
     */
    char expectStringAsync(const char* str, unsigned short timeout);
    
    bool setMacParam(const char* paramName, const unsigned char* paramValue, unsigned short size);
    bool setMacParam(const char* paramName, unsigned char paramValue);
    bool setMacParam(const char* paramName, const char* paramValue);
    
    /**
     * Send and wait for response
     */
    unsigned char macTransmit(const char* type, const unsigned char* payload, unsigned char size);
    
    /**
     * Send command
     */
    void macSendCommand(const char* type, const unsigned char* payload, unsigned char size);
    
    /**
     * Convert the text value for spreading factor into a number between 0 and 6
     */
    int sfToIndex(char* value);
    
    unsigned char macTransmitGetResponse();
    
    /**
     * Checks the current input string against the param. Returns true if they match
     */
    char checkInputInstring(const char* str);
};

#endif