#include "OTAACredentials.h"

OTAACredentials::OTAACredentials(uint8_t *deviceEUI, uint8_t *applicationEUI, uint8_t *applicationKey)
{
    setDeviceEUI(deviceEUI);
    setApplicationEUI(applicationEUI);
    setApplicationKey(applicationKey);
}

void OTAACredentials::setDeviceEUI(uint8_t *deviceEUI)
{
	this->deviceEUI = deviceEUI;
}

void OTAACredentials::setApplicationEUI(uint8_t *applicationEUI)
{
	this->applicationEUI = applicationEUI;
}

void OTAACredentials::setApplicationKey(uint8_t *applicationKey)
{
	this->applicationKey = applicationKey;
}

const unsigned char *OTAACredentials::getDeviceEUI()
{
	return deviceEUI;
}

const unsigned char *OTAACredentials::getApplicationEUI()
{
	return applicationEUI;
}

const unsigned char *OTAACredentials::getApplicationKey()
{
	return applicationKey;
}
