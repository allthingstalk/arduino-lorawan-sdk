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

To use the container format, simply define a `Container` object at the start of your sketch.

```
Container container(device);
```
```
container.addToQueue(counter, INTEGER_SENSOR, false);
device.processQueue();
```

### Custom binary payload

Using the custom payload builder, you can send data from multiple sources in one payload. Make sure you set the correct decoding file at AllThingsTalk.
For more information, please visit [custom-payload-conversion](http://docs.allthingstalk.com/developers/data/custom-payload-conversion/) in our documentation.

To use the custom payload format, simply define a `PayloadBuilder` object at the start of your sketch.

```
PayloadBuilder payload(device);
```

To add more data to your payload, simply use the `addType(value)` functions

* `addBoolean(uint8_t value)`
* `addInteger(int value)`
* `addNumber(float value)`
* `addGPS(float latitude, float longitude, float altitude)`
* `addAccelerometer(float x, float y, float z)`

> Reset the payload before building a new one!

```
payload.reset();
payload.addInteger(counter);
payload.addToQueue(false);
device.processQueue();
```

> Set the correct decoding file at AllThingsTalk so your data is decoded correctly.

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

### Handling the queue

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