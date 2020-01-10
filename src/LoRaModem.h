#ifndef LORA_MODEM_H_
#define LORA_MODEM_H_

#include "Arduino.h"
#include "Device.h"
#include "Payload.h"
#include "BinaryPayload.h"
#include "ABPCredentials.h"
#include "OTAACredentials.h"
#include "Options.h"
#include "LoRaOptions.h"

#include <stdint.h>

enum LoRaCredentialsType { unknown, abp, otaa };

struct LoRaDownlink {
    BinaryPayload payload;
    LoRaOptions options;
};

class LoRaModem : public Device<LoRaOptions> {
public:
    LoRaModem(HardwareSerial &loraSerial, Stream &debugStream);
    LoRaModem(HardwareSerial &loraSerial, Stream &debugStream, ABPCredentials &credentials);
    LoRaModem(HardwareSerial &loraSerial, Stream &debugStream, OTAACredentials &credentials);

    bool init(ABPCredentials &abpCredentials);
    bool init(OTAACredentials &otaaCredentials);
    bool init();

    unsigned int setPort(unsigned int port);
    void setAck(bool ack);

    int setSpreadingFactor(int spreadingFactor);
    bool setAdaptiveDataRate(bool adr);

    char *getSysParam(const char *name, unsigned short timeout = defaultTimeout);
    char *getMacParam(const char *name, unsigned short timeout = defaultTimeout);
    char *getRadioParam(const char *name, unsigned short timeout = defaultTimeout);

    char *getHwEui();
    char *getModemVersion();
    char *getFrequencyBand();
    char *getAdaptiveDataRate();
    char *getDataRate();
    char *getStatus();
    char *getSpreadingFactor();
    char *getModulationMode();
    char *getOperationFrequency();

    bool reset(unsigned int retries = 3);
    void wakeUp();
    void sleep(uint32_t milliseconds = 60000);

    unsigned int getDefaultBaudRate();
    unsigned int getMaxPayloadSize();

    char *getLastErrorCode();
    char *humanizeErrorCode(char *errorCode);

    bool send(Payload &payload);

    using Device<LoRaOptions>::send;
    using Device<LoRaOptions>::setDownlinkCallback;

private:
    static const unsigned int maxPayloadSize = 240;
    static const unsigned int defaultInputBufferSize = 440;
    static const unsigned int defaultTimeout = 120;

    template<typename T> void log(T message, char separator = '\n');
    void logError(char *errorCode);

    bool connect(ABPCredentials &abpCredentials);
    bool connect(OTAACredentials &otaaCredentials);

    char *readln(unsigned int timeout = 1000); // Default RN2483 value
    bool expectString(char *str, unsigned int timeout = defaultTimeout);
    bool expectOk(unsigned int timeout = 1000);
    bool expectAccepted(unsigned int timeout = 1000);

    bool receive();
    LoRaDownlink *parseMacRx(char *macRx);

    char *getParam(const char *type, const char *name, unsigned short timeout = defaultTimeout);

    void setParamProlog(const char *type, const char *name);
    template<typename T> bool setParam(const char *type, const char *name, T value);
    bool setParam(const char *type, const char *name, const unsigned char *value, unsigned int size);
    template<typename T> bool setSysParam(const char *name, T value);
    bool setSysParam(const char *name, const unsigned char *value, unsigned int size);
    template<typename T> bool setMacParam(const char *name, T value);
    bool setMacParam(const char *name, const unsigned char *value, unsigned int size);
    template<typename T> bool setRadioParam(const char *name, T value);
    bool setRadioParam(const char *name, const unsigned char *value, unsigned int size);

    bool adr = false; // Adaptive data rate
    bool isRN2903 = false; // If the modem is the US RN2903 model

    char inputBuffer[defaultInputBufferSize + 1];
    char lastErrorCode[64];

    HardwareSerial *loraSerial;
    Stream *debugStream;

    LoRaCredentialsType credentialsType;
    ABPCredentials abpCredentials;
    OTAACredentials otaaCredentials;
};

#endif
