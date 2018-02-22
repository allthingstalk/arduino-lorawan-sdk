# arduino-lorawan-sdk

This is a SDK by AllThingsTalk that provides connectivity to their cloud through [LoRaWan radios](https://www.lora-alliance.org/What-Is-LoRa/Technology).  

### Version v2.1

What's new?

* Support for CBOR messages

## Hardware

This SDK has been tested to work with the following hardware

Chip
- [Microchip](http://www.microchip.com/wwwproducts/Devices.aspx?product=RN2483) RN 2483

Board
- Sodaq Mbili
- Sodaq ONE

## Installation

Download the source code and copy the content of the zip file to your arduino libraries folder (usually found at /libraries) _or_ import the .zip file directly using the Arduino IDE.

### Settings keys

Open the `keys.h` file and fill in your device credentials. You can copy paste them from your device at AllThingsTalk in the _SETTINGS > Connectivity_ tab.

```
#ifndef KEYS_h
#define KEYS_h

uint8_t DEV_ADDR[4] = {0x01, 0xD8, ..., ...};
uint8_t APPSKEY[16] = {0x38, 0x4F, 0x94, 0x1B, 0x30, 0xFE, 0xF3, 0xF5, ..., ..., ..., 0x9A, 0x80, 0xC1, 0x9B, 0x3E};
uint8_t NWKSKEY[16] = {0x66, 0x3F, 0xB3, 0x67, 0xFF, 0xE2, 0x4E, 0x71, ..., ..., ..., 0x2B, 0x24, 0xF1, 0x77, 0x45};

#endif
```

### Payloads

There are three ways to send your data to AllThingsTalk

* `Standardized containers`
* `Cbor payload`
* `Binary payload`

Containers will send a single datapoint to a single asset. Both _Cbor_ and _Binary_ allow you to construct your own payload. The former is slightly larger in size, the latter requires a small decoding file [(example)](https://github.com/allthingstalk/arduino-lorawan-sdk/blob/master/examples/counter/counter-payload-builder.json) on the receiving end.

You can simply select the method you prefer by (un)commenting the methods at the start of the sketch.

```
// Select your preferred method of sending data
//#define CONTAINERS
#define CBOR
//#define BINARY
```

#### Containers

Send a single datapoint to a single asset using the `send(value, asset)` functions. Value can be any primitive type `integer`, `float`, `boolean` or `String`. For example

```
ATTDevice device(&modem, &debugSerial, false);
Container container(device);
```
```
  container.addToQueue(counter, INTEGER_SENSOR, false);
```

#### Cbor

```
ATTDevice device(&modem, &debugSerial, false);
CborBuilder payload(device);  // Construct a payload object
```
```
  payload.reset();
  payload.map(1);  // Set number of datapoints in payload
  payload.addInteger(25, "15");
  payload.send();
```

#### Binary payload

Using the [AllThingsTalk ABCL language](http://docs.allthingstalk.com/developers/custom-payload-conversion/), you can send a binary string containing datapoints of multiple assets in a single message. The example below shows how you can easily construct and send your own custom payload.

> Make sure you set the correct decoding file at AllThingsTalk. Please check our documentation and the included experiments for examples.

```
ATTDevice device(&modem, &debugSerial, false);
PayloadBuilder payload(device);  // Construct a payload object
```
```
  payload.reset();
  payload.addInteger(25);
  payload.addNumber(false);
  payload.addNumber(3.1415926);
  payload.send();
```

### Examples

Basic example showing all fundamental parts to set up a working example. Send data from the device, over NB-IoT to AllThingsTalk.

* `counter` This example shows how you can send over a simple integer counter using either _json_, _cbor_ or a _binary payload_.

Simply uncomment your selected method for sending data at the top of the sketch.

```
// Uncomment your selected method for sending data
#define CONTAINERS
//#define CBOR
//#define BINARY
```

* `instrumentation` Print out Modem parameters

> When running the examples, make sure you select the correct hardware at the top of the sketch.

```
// Select your hardware
#define SODAQ_MBILI
//#define SODAQ_ONE
```

### Managing the queue

`processQueue()` will return a number based on the success of sending the message.
In case of a failure (`-1`), you can remove the message from the queue if wanted using `pop()`.

```
sendState = device.processQueue();

if(sendState == -1)
{
  debugSerial.println("Failed to send data; removing from queue");
  device.pop();
}

```

### ACK / Acknowledgement

In all `addToQueue` functions, we can set an extra boolean parameter to enable or disable acknowledgements. By default this is set to _true_. We wait for a response so we are sure our message went through. If set to _false_, we do not wait for a response.

```
container.addToQueue(counter, INTEGER_SENSOR, false);  // don't wait for ACK
```
```
payload.addToQueue(false);  // don't wait for ACK
```