#ifndef DEVICE_H_
#define DEVICE_H_

class Payload;
class BinaryPayload;
class CborPayload;

template <class Options>
class Device {
public:
    virtual void setOptions(Options &options);
    virtual Options getOptions();

    virtual bool send(Payload &payload) = 0;
    virtual bool send(char *str);
    virtual bool send(Payload &payload, Options &options);
    virtual bool send(char *str, Options &options);

    virtual void setDownlinkCallback(void (*callback)(BinaryPayload &payload, Options &options));

protected:
    Options options;
    void (*callback)(BinaryPayload &payload, Options &options) = nullptr;
};

#endif
