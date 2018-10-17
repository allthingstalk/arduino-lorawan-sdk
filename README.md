# arduino-lorawan-sdk

This is a SDK by AllThingsTalk that provides connectivity to their cloud through [LoRaWan radios](https://www.lora-alliance.org/What-Is-LoRa/Technology).

### Version v2.2

What's new?

* Support for Ports when sending messages
* Support OTAA
* Support Actuation
* Support for setting the Spreading factor

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

For ABP
```
#ifndef KEYS_h
#define KEYS_h

uint8_t DEV_ADDR[4] = {0x01, 0xD8, ..., ...};
uint8_t APPSKEY[16] = {0x38, 0x4F, 0x94, 0x1B, 0x30, 0xFE, 0xF3, 0xF5, ..., ..., ..., 0x9A, 0x80, 0xC1, 0x9B, 0x3E};
uint8_t NWKSKEY[16] = {0x66, 0x3F, 0xB3, 0x67, 0xFF, 0xE2, 0x4E, 0x71, ..., ..., ..., 0x2B, 0x24, 0xF1, 0x77, 0x45};

#endif
```

For OTAA
```
#ifndef KEYS_h
#define KEYS_h

uint8_t DEV_EUI[8] = { 0x00, 0x6F, 0xF1, ..., ..., ..., 0xF6, 0xEA };
uint8_t APPS_EUI[8] = { 0x70, 0xB3, 0xD5, ..., ..., ..., 0x37, 0x2D };
uint8_t APP_KEY[16] = { 0x66, 0x58, 0xC5, 0x69, 0x81, 0xCC, 0xA2, ..., ..., ..., 0xDA, 0xC5, 0x80, 0xF5, 0x2E, 0x18 };

#endif
```
### Connecting

The connection for ABP and OTAA is a little different.

#### For ABP
```
MicrochipLoRaModem modem(&loraSerial, &debugSerial);
ATTDevice device(&modem, &debugSerial, false, 7000);

void setup()
{
  debugSerial.begin(SERIAL_BAUD);
  while((!debugSerial) && (millis()) < 10000){}  // wait until the serial bus is available

  loraSerial.begin(modem.getDefaultBaudRate());  // set baud rate of the serial connection to match the modem
  while((!loraSerial) && (millis()) < 10000){}   // wait until the serial bus is available

  while(!device.initABP(DEV_ADDR, APPSKEY, NWKSKEY));
  debugSerial.println("Ready to send data");
}
```

#### For OTAA
```
MicrochipLoRaModem modem(&loraSerial, &debugSerial);
ATTDevice device(&modem, &debugSerial, false, 7000);

void setup()
{
  debugSerial.begin(SERIAL_BAUD);
  while((!debugSerial) && (millis()) < 10000){}  // wait until the serial bus is available

  loraSerial.begin(modem.getDefaultBaudRate());  // set baud rate of the serial connection to match the modem
  while((!loraSerial) && (millis()) < 10000){}   // wait until the serial bus is available

  while(!device.initOTAA(DEV_EUI, APPS_EUI, APP_KEY, false));
  debugSerial.println("Ready to send data");
}

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

* `basicActuation` This examples shows you how can receive actuation messages

*On constrained devices, such as your class A LoraWAN device (https://www.thethingsnetwork.org/docs/lorawan/#end-devices), saving the energy is paramount. This is why the radio is mostly turned off, while the device itself spends time asleep. The downside to this is that you cannot receive actuations (downlink messages) at any time, but only in those short periods of time when the radio is listening. On class A devices, by standard, these short periods occur immediately after the radio sends a message. This is why we’re using a button in this example - to be able to trigger the radio and allow for downlink messages to arrive.*

```
void callback(const unsigned char* payload, unsigned int length, uint8_t port );
MicrochipLoRaModem modem(&loraSerial, &debugSerial, callback);
ATTDevice device(&modem, &debugSerial, false, 7000);

bool sensorVal = false;
bool prevButtonState = false;

void loop()
{
  bool sensorRead = digitalRead(digitalSensor);     // Read status Digital Sensor
  if (sensorRead == 1 && prevButtonState == false)  // Verify if value has changed
  {
    prevButtonState = true;
    debugSerial.println("Button pressed");
    sendValue(!sensorVal);
    sensorVal = !sensorVal;
  }
  else if(sensorRead == 0)
    prevButtonState = false;
}

// callback function
// handle messages that were sent from the AllThingsTalk cloud to this device
void callback(const unsigned char* payload, unsigned int length, uint8_t port )
{
   String msgString;

   char message_buff[length + 1];
   strncpy(message_buff, (char*)payload, length);
   message_buff[length] = '\0';
   msgString = String(message_buff);

   Serial.print("Payload: ");
   Serial.println(msgString);

}
```

* `instrumentation` Print out Modem parameters

> When running the examples, make sure you select the correct hardware at the top of the sketch.

```
// Select your hardware
#define SODAQ_MBILI
//#define SODAQ_ONE
```

### Managing the queue

#### Without port number
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
#### With port number
You can also use  processQueue with a port number for saying I want to send data on that port.

```
sendState = device.processQueue(3);

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

# License
Apache 2.0

# Contributions
Pull requests and new issues are welcome.
