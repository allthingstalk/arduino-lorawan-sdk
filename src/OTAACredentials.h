#ifndef OTAA_CREDENTIALS_H_
#define OTAA_CREDENTIALS_H_

#include "Credentials.h"
#include <stdint.h>

class OTAACredentials : public Credentials {
public:
    OTAACredentials(uint8_t* deviceEUI = nullptr, uint8_t* applicationEUI = nullptr, uint8_t* applicationKey = nullptr);

    void setDeviceEUI(uint8_t* deviceEUI);
    void setApplicationEUI(uint8_t *applicationEUI);
    void setApplicationKey(uint8_t* applicationKey);

    virtual const unsigned char* getDeviceEUI();
    virtual const unsigned char* getApplicationEUI();
    virtual const unsigned char* getApplicationKey();

private:
    uint8_t* deviceEUI;
    uint8_t* applicationEUI;
    uint8_t* applicationKey;
};

#endif
