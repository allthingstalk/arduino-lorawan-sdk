#include "Utils.h"
#include "LoRaModem.h"
#include "LoRaOptions.h"
#include "BinaryPayload.h"

// As per microchip specs at:
// https://ww1.microchip.com/downloads/en/DeviceDoc/40001784B.pdf

LoRaModem::LoRaModem(HardwareSerial &loraSerial, Stream &debugStream) : Device(), abpCredentials(), otaaCredentials()
{
    this->lastErrorCode[0] = 0;
    this->loraSerial = &loraSerial;
    this->debugStream = &debugStream;
    this->credentialsType = unknown;
}

LoRaModem::LoRaModem(HardwareSerial &loraSerial, Stream &debugStream, ABPCredentials &abpCredentials) : Device(), abpCredentials(), otaaCredentials()
{
    this->lastErrorCode[0] = 0;
    this->loraSerial = &loraSerial;
    this->debugStream = &debugStream;
    this->credentialsType = abp;
    this->abpCredentials = abpCredentials;
}

LoRaModem::LoRaModem(HardwareSerial &loraSerial, Stream &debugStream, OTAACredentials &otaaCredentials) : Device(), abpCredentials(), otaaCredentials()
{
    this->lastErrorCode[0] = 0;
    this->loraSerial = &loraSerial;
    this->debugStream = &debugStream;
    this->credentialsType = otaa;
    this->otaaCredentials = otaaCredentials;
}

template<typename T> void LoRaModem::log(T message, char separator) {
    if (debugStream) {
        debugStream->print(message);
        if (separator) {
            debugStream->print(separator);
        }
    }
}

char *LoRaModem::getLastErrorCode() {
    return lastErrorCode;
}

char *LoRaModem::humanizeErrorCode(char *errorCode) {
    char *errDict[10][2] = {
        { "invalid_param", "An invalid parameter was sent to the modem." },
        { "keys_not_init", "The lora keys corresponding to the join mode (otaa or abp) were not properly configured." },
        { "no_free_ch", "All LoRa channels are currently busy." },
        { "silent", "The device is in silent Immediately state." },
        { "busy", "The MAC state is not in an idle state." },
        { "mac_paused", "The MAC has been paused and not resumed back." },
        { "not_joined", "The network is not joined." },
        { "frame_counter_err_rejoin_needed", "The frame counter rolled over, possible rejoin required" },
        { "invalid_data_len", "The application payload length is greater than the maximum application payload length corresponding to the current data rate" },
        { "mac_err", "ACK not received back from the server" }
    };
    for (auto i = 0; i < 10; ++i) {
        if (strcmp(errorCode, errDict[i][0]) == 0) {
            return errDict[i][1];
        }
    }
    return "Unknown";
}

void LoRaModem::logError(char *errorCode) {
    log("ERROR", ' ');
    log(errorCode, ':');
    log("", ' ');
    log(humanizeErrorCode(errorCode));
}

bool LoRaModem::init(ABPCredentials &abpCredentials) {
    this->credentialsType = abp;
    this->abpCredentials = abpCredentials;
    return init();
}

bool LoRaModem::init(OTAACredentials &otaaCredentials) {
    this->credentialsType = otaa;
    this->otaaCredentials = otaaCredentials;
    return init();
}

bool LoRaModem::init() {
    log("Initializing...");
    loraSerial->begin(getDefaultBaudRate());
    while (!loraSerial) {}
    if (!reset()) {
        log("Reset failed.");
        return false;
    }
    if (credentialsType == abp) {
        return connect(abpCredentials);
    }
    if (credentialsType == otaa) {
        return connect(otaaCredentials);
    }

    log("Warning: No keys set during initialization.");
    return true;
}

bool LoRaModem::connect(ABPCredentials &abpCredentials) {
    if (!setMacParam("devaddr", abpCredentials.getDeviceAddress(), 4))
        return false;

    if (!setMacParam("appskey", abpCredentials.getApplicationSessionKey(), 16))
        return false;

    if (!setMacParam("nwkskey", abpCredentials.getNetworkSessionKey(), 16))
        return false;

    loraSerial->print("mac join abp\r\n");
    return expectOk() && expectAccepted(30000);
}


bool LoRaModem::connect(OTAACredentials &otaaCredentials) {
    if (!setMacParam("deveui", otaaCredentials.getDeviceEUI(), 8))
        return false;

    if (!setMacParam("appeui", otaaCredentials.getApplicationEUI(), 8))
        return false;

    if (!setMacParam("appkey", otaaCredentials.getApplicationKey(), 16))
        return false;

    loraSerial->print("mac join otaa\r\n");
    return expectOk() && expectAccepted(30000);
}

bool LoRaModem::reset(unsigned int retries) {
    log("Resetting the modem.");
    loraSerial->print("sys reset\r\n");
    auto in = readln();
    if (strlen(in) > 0) {
        log("Received:", ' ');
        log(in);
        if (strstr(in, "RN2483") != nullptr) {
            log("Found RN2483 (European).");
            isRN2903 = false;
            return true;
        }
        if (strstr(in, "RN2903") != nullptr) {
            log("Found RN2903 (US).");
            isRN2903 = true;
            return true;
        }
    } else {
        if (retries == 0) {
            return false;
        }
        log("Reset failed, retrying.");
        sleep(100);
        wakeUp();
        reset(retries - 1);
    }
}

void LoRaModem::sleep(uint32_t milliseconds) {
    log("Putting the modem into sleep mode.");
    loraSerial->print("sys sleep ");
    if (milliseconds < 100) milliseconds = 100;
    loraSerial->print(milliseconds);
    loraSerial->print("\r\n");
}

// Taken from https://github.com/SodaqMoja/Sodaq_RN2483
void LoRaModem::wakeUp() {
    log("Waking up the modem.");

    // Emulating break condition.
    loraSerial->flush();
    loraSerial->begin(300);
    loraSerial->write((uint8_t)0x00);
    loraSerial->flush();

    delay(50);

    // Setting baudrate
    loraSerial->begin(getDefaultBaudRate());
    loraSerial->write((uint8_t)0x55);
    loraSerial->flush();

    readln();
}

unsigned int LoRaModem::getDefaultBaudRate() {
    return 57600;
}

unsigned int LoRaModem::getMaxPayloadSize() {
    return maxPayloadSize;
}

unsigned int LoRaModem::setPort(unsigned int port) {
    if (port <= 0 || port > 223) {
        log("Port not in expected range (1-223). Port 1 will be used instead.");
        options.port = 1;
    } else {
        options.port = port;
    }
    return options.port;
}

void LoRaModem::setAck(bool ack) {
    this->options.ack = ack;
}

int LoRaModem::setSpreadingFactor(int spreadingFactor) {
    if (!isRN2903 && spreadingFactor >= 7 && spreadingFactor <= 12) {
        // EU regulations pass
    } else if (isRN2903 && spreadingFactor >= 7 && spreadingFactor <= 10) {
        // US regulations pass
    } else {
        // Invalid spreading factor
        log("Spreading factor not in expected range (EU 7-12, US 7-10). Using default - 7.");
        spreadingFactor = 7;
    }

    // Send to modem
    int8_t datarate;
    if (setMacParam("adr", "off")) {
        setMacParam("dr", (isRN2903 ? 10 : 12) - spreadingFactor);
    }

    return spreadingFactor;
}

bool LoRaModem::setAdaptiveDataRate(bool adr) {
    if (setMacParam("adr", adr ? "on" : "off")) {
        return adr;
    } else {
        return strstr(getAdaptiveDataRate(), "on") != nullptr;
    }
}

bool LoRaModem::send(Payload &payload) {
    char buffer[3]; // hex conversion buffer

    if (payload.getSize() > getMaxPayloadSize()) {
        log("Can't send the payload, size too big.");
        return false;
    }

    // Start sending payload.
    log("Sending:", ' ');
    loraSerial->print("mac tx ");
    loraSerial->print(options.ack ? "cnf " : "uncnf ");
    loraSerial->print(options.port);
    loraSerial->print(" ");

    unsigned char *bytes = payload.getBytes();
    for (auto i = 0; i < payload.getSize(); ++i) {
        sprintf(buffer, "%.2X", bytes[i]);
        log(buffer, ' ');
        loraSerial->print(buffer);
    }
    log("");
    loraSerial->print("\r\n");

    // Wait until payload is sent.
    log("Waiting...");
    if (!expectOk()) {
        return false;
    }

    return receive();
}

bool LoRaModem::receive() {
    char *response = readln(30000); // two windows
    if (strstr(response, "mac_tx_ok") != nullptr) {
        log("Received mac_tx_ok");
        return true; // no (more) downlink
    } else if (strstr(response, "ok") != nullptr) {
        log("Received ok");
        return receive();
    } else if (strstr(response, "mac_rx") != nullptr) {
        log("Received mac_rx (downlink found)");
        if (callback != nullptr) {
            auto downlink = parseMacRx(response);
            callback(downlink->payload, downlink->options);
            free(downlink);
        } else {
            log("No downlink callback set.");
        }
        readln(4000); // is this even needed?
        return true;
    } else {
        logError(response);
        return false;
    }
}

LoRaDownlink *LoRaModem::parseMacRx(char *macRx) {
    auto downlink = new LoRaDownlink();

    char convBuffer[3]; // hex conversion buffer

    // discard "mac_rx"
    strtok(macRx, " ");

    // get port
    char *port = strtok(nullptr, " ");
    downlink->options.port = strtol(port, nullptr, 10);

    // get payload
    char *payload = strtok(nullptr, " ");
    unsigned int payloadLength = strlen(payload);
    auto buffer = new unsigned char[(strlen(payload) / 2)];

    for (auto i = 0; i < payloadLength; i += 2) {
        strncpy(convBuffer, payload + i, 2);
        buffer[i/2] = strtol(convBuffer, nullptr, 16);
    }

    BinaryPayload binaryPayload(buffer, (strlen(payload) / 2));
    downlink->payload = binaryPayload;
    return downlink;
}

char *LoRaModem::getHwEui() {
    return getSysParam("hweui");
}

char *LoRaModem::getModemVersion() {
    return getSysParam("ver");
}

char *LoRaModem::getFrequencyBand() {
    return getMacParam("band");
}

char *LoRaModem::getAdaptiveDataRate() {
    return getMacParam("adr");
}

char *LoRaModem::getDataRate() {
    return getMacParam("dr");
}

char *LoRaModem::getStatus() {
    return getMacParam("status");
}

char *LoRaModem::getSpreadingFactor() {
    return getRadioParam("sf");
}

char *LoRaModem::getModulationMode() {
    return getRadioParam("mod");
}

char *LoRaModem::getOperationFrequency() {
    return getRadioParam("freq");
}

char *LoRaModem::getSysParam(const char* name, unsigned short timeout) {
    return getParam("sys", name, timeout);
}

char *LoRaModem::getMacParam(const char* name, unsigned short timeout) {
    return getParam("mac", name, timeout);
}

char *LoRaModem::getRadioParam(const char* name, unsigned short timeout) {
    return getParam("radio", name, timeout);
}

char *LoRaModem::getParam(const char *type, const char *name, unsigned short timeout) {
    loraSerial->print(type);
    loraSerial->print(" get ");
    loraSerial->print(name);
    loraSerial->print("\r\n");
    return readln(timeout);
}

void LoRaModem::setParamProlog(const char *type, const char *name) {
    log("Setting", ' ');
    log(type, ' ');
    log("param", ' ');
    log(name, ' ');
    log("to", ' ');
    loraSerial->print(type);
    loraSerial->print(" set ");
    loraSerial->print(name);
    loraSerial->print(" ");
}

template<typename T> bool LoRaModem::setParam(const char *type, const char *name, T value) {
    setParamProlog(type, name);
    log(value);
    loraSerial->print(value);
    loraSerial->print("\r\n");
    return expectOk();
}

bool LoRaModem::setParam(const char *type, const char *name, const unsigned char *value, unsigned int size) {
    char buffer[3]; // hex conversion buffer
    setParamProlog(type, name);
    for (auto i = 0; i < size; ++i) {
        sprintf(buffer, "%.2X", value[i]);
        log(buffer, ' ');
        loraSerial->print(buffer);
    }
    log("");
    loraSerial->print("\r\n");
    return expectOk();
}

template<typename T> bool LoRaModem::setSysParam(const char *name, T value) {
    return setParam("sys", name, value);
}

bool LoRaModem::setSysParam(const char *name, const unsigned char *value, unsigned int size) {
    return setParam("sys", name, value, size);
}

template<typename T> bool LoRaModem::setMacParam(const char *name, T value) {
    return setParam("mac", name, value);
}

bool LoRaModem::setMacParam(const char *name, const unsigned char *value, unsigned int size) {
    return setParam("mac", name, value, size);
}

template<typename T> bool LoRaModem::setRadioParam(const char *name, T value) {
    return setParam("radio", name, value);
}

bool LoRaModem::setRadioParam(const char *name, const unsigned char *value, unsigned int size) {
    return setParam("radio", name, value, size);
}

char *LoRaModem::readln(unsigned int timeout) {
    loraSerial->setTimeout(timeout);
    unsigned int len = loraSerial->readBytesUntil('\n', inputBuffer, defaultInputBufferSize);

    if (len > 0) {
        inputBuffer[len - 1] = 0;  // remove trailing \r
    } else {
        inputBuffer[0] = 0;
    }
    return inputBuffer;
}

bool LoRaModem::expectString(char *str, unsigned int timeout) {
    char *line = readln(timeout);
    if (strstr(line, str)) {
        return true;
    } else {
        logError(line);
        return false;
    }
}

bool LoRaModem::expectOk(unsigned int timeout) {
    return expectString("ok", timeout);
}

bool LoRaModem::expectAccepted(unsigned int timeout) {
    return expectString("accepted", timeout);
}