# arduino-lorawan-sdk

This is a SDK by AllThingsTalk that provides connectivity to their cloud through [LoRa radios](https://www.lora-alliance.org/What-Is-LoRa/Technology).  

## Hardware

This SDK has been tested to work with the following hardware

Chip
- [Microchip](http://www.microchip.com/wwwproducts/Devices.aspx?product=RN2483)

Board
- Sodaq Mbili
- Sodaq ONE

## Installation

Download the source code and copy the content of the zip file to your arduino libraries folder (usually found at /libraries) _or_ import the .zip file directly using the Arduino IDE.

## Examples

Two version of a basic example on how to get data to the AllThingsTalk Cloud are included. These examples work for both the Sodaq Mbili and Sodaq ONE. No other hardware required.
* `counter-containers` counter using preset [container definitions](http://docs.allthingstalk.com/developers/data/default-payload-conversion/)
* `counter-payload-builder` counter using our [custom binary payload decoding](http://docs.allthingstalk.com/developers/data/custom-payload-conversion/)

A third example is included to show how to print out Modem parameters
* `instrumentation`

> When running the examples, make sure you (un)comment the correct Serial communication in the sketch
```
// Sodaq ONE
#define debugSerial SerialUSB
#define loraSerial Serial1

// Sodaq Mbili
//#define debugSerial Serial
//#define loraSerial Serial1
```

## Sending data

### Containers

```
Container container(Device);

container.AddToQueue(counter, INTEGER_SENSOR);

Device.ProcessQueue();
```

### Custom binary payload

Using the custom payload builder, you can send data from multiple sources in one payload. Make sure you set the correct decoding file at AllThingsTalk.
For more information, please visit [custom-payload-conversion](http://docs.allthingstalk.com/developers/data/custom-payload-conversion/) in our documentation.

```
static uint8_t sendBuffer[51];
ATT_PB payload(51);  // buffer is set to the same size as the sendBuffer[]

payload.reset();
payload.addInteger(counter);
payload.copy(sendBuffer);

Device.AddToQueue(&sendBuffer, payload.getSize(), false);
Device.ProcessQueue();
```

Example decoding json

> Make sure you have an `integer` asset with the correct **name** (the name you set in the decoding json) under your device in AllThingsTalk

```
{
  "sense": [
    {
      "asset": "counter",
      "value": {"byte": 0, "type": "integer"}
    }
  ]
}
```