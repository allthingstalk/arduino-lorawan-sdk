#include "ABPCredentials.h"

ABPCredentials::ABPCredentials(uint8_t *deviceAddress, uint8_t *applicationSessionKey, uint8_t *networkSessionKey) {
    setDeviceAddress(deviceAddress);
    setApplicationSessionKey(applicationSessionKey);
    setNetworkSessionKey(networkSessionKey);
}

void ABPCredentials::setDeviceAddress(uint8_t *deviceAddress) {
    this->deviceAddress = deviceAddress;
}

void ABPCredentials::setApplicationSessionKey(uint8_t *applicationSessionKey) {
    this->applicationSessionKey = applicationSessionKey;
}

void ABPCredentials::setNetworkSessionKey(uint8_t *networkSessionKey) {
    this->networkSessionKey = networkSessionKey;
}

unsigned char *ABPCredentials::getDeviceAddress() {
	return deviceAddress;
}

unsigned char *ABPCredentials::getApplicationSessionKey() {
	return applicationSessionKey;
}

unsigned char *ABPCredentials::getNetworkSessionKey() {
	return networkSessionKey;
}
