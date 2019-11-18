#include "Device.h"
#include "LoRaOptions.h"
#include "CborPayload.h"
#include "BinaryPayload.h"

#include <string.h>

template<class Options>
void Device<Options>::setOptions(Options &options) {
    this->options = options;
}

template<class Options>
Options Device<Options>::getOptions() {
    return options;
}

template<class Options>
bool Device<Options>::send(char *str) {
    auto payload = BinaryPayload(reinterpret_cast<unsigned char *>(str), strlen(str));
    return send(payload);
}

template<class Options>
bool Device<Options>::send(Payload &payload, Options &options) {
    Options previousOptions = getOptions();
    setOptions(options);
    send(payload);
    setOptions(previousOptions);
}

template<class Options>
bool Device<Options>::send(char *payload, Options &options) {
    Options previousOptions = getOptions();
    setOptions(options);
    send(payload);
    setOptions(previousOptions);
}

template<class Options>
void Device<Options>::setDownlinkCallback(void (*callback)(BinaryPayload &payload, Options &options)) {
    this->callback = callback;
}

template class Device<LoRaOptions>;