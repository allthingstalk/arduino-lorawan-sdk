#ifndef ABP_CREDENTIALS_H_
#define ABP_CREDENTIALS_H_

#include "Credentials.h"
#include <stdint.h>

class ABPCredentials : public Credentials {
public:
    ABPCredentials(uint8_t *deviceAddress = nullptr, uint8_t *applicationSessionKey = nullptr, uint8_t *networkSessionKey = nullptr);

    void setDeviceAddress(uint8_t *deviceAddress);
    void setApplicationSessionKey(uint8_t *applicationSessionKey);
    void setNetworkSessionKey(uint8_t *networkSessionKey);

    unsigned char *getDeviceAddress();
    unsigned char *getApplicationSessionKey();
    unsigned char *getNetworkSessionKey();

private:
    uint8_t *deviceAddress;
    uint8_t *applicationSessionKey;
    uint8_t *networkSessionKey;
};

#endif
