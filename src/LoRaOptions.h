#ifndef LORA_OPTIONS_H_
#define LORA_OPTIONS_H_

#include "Options.h"

class LoRaOptions : public Options {
public:
    LoRaOptions(int port = 1, bool ack = false) : port(port), ack(ack) { }

    int port = 1;
    bool ack = false;
};

#endif
