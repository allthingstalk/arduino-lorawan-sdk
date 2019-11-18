

# AllThingsTalk Arduino LoRaWAN SDK

<img align="right" width="300" height="200" src="extras/lora-logo.png">

AllThingsTalk Arduino Library for LoRa Devices - makes connecting devices with [AllThingsTalk Maker](https://maker.allthingstalk.com/) a breeze.  

> [AllThingsTalk](https://www.allthingstalk.com) is an accessible IoT Platform for rapid development.  
In the blink of an eye, you'll be able to extract, visualize and use the collected data.  
[Get started and connect up to 10 devices free-of-charge](https://www.allthingstalk.com/maker)

# Installing

This library is available on Arduino Library Manager.  
To download (or update), simply open your Arduino IDE, go to *Tools > Manage Libraries*, search for and install "*AllThingsTalk LoRaWAN SDK*".

> In case you already have this SDK installed (from before it was available in Arduino Library Manager), please go to your "libraries" folder and delete (or backup) it before updating/installing it from Arduino Library Manager.

## Version History

> **Note:** To avoid conflicts when updating, please remove or backup the old SDK version.

### 3.1.0

- Unified all libraries into a single "*AllThingsTalk_LoRaWAN.h*" (from now on, that's all you have to include to your sketch to use this SDK)
- Updated all examples to use *AllThingsTalk_LoRaWAN.h*
- Refined all [RDK](/examples/RDK) examples  (with comments on every line)
- Added README files for all [RDK](/examples/RDK) examples

### 3.0.0

- Redesigned SDK structure
- CBORPayload & BinaryPayload formalized
- LoraOptions in downlink callbacks
- Modem initialization simplified
- Message queue management removed
- Debug information extended
- More examples

### 2.2.0

- Support for Ports when sending messages
- Support OTAA
- Support Actuation
- Support for setting the Spreading factor

### 2.1.0

- Support for CBOR messages

# Hardware

This SDK has been tested to work with the following hardware:

Chip
- [Microchip](http://www.microchip.com/wwwproducts/Devices.aspx?product=RN2483) RN 2483

Board
- Sodaq Mbili
- Sodaq ONE


# Classes

## Credentials
New classes are introduced, like the credentials classes.  For LoRa you have two new classes, the ABP Credential class and the OTAA Credential class.  Both classes can be used as parameter voor the LoraModem (see _Modems_)

### ABP Credentials
The ABP Credential class exists of one constructor with following parameters: Device Address, Application Session Key and Network Session Key.
It also has the functions setDeviceAddress, setApplicationSessionKey, setNetworkSessionKey, getDeviceAddress, getApplicationSessionKey and getNetworkSessionKey.
This class can be used as parameter for a Modem class

### OTAA Credentials
The OTAA Credential class exists also of one constructor with parameters like Device EUI, Application EUI and Application Key.
Like the ABP Credentials class it also has function for setting and retrieving DeviceEUI, ApplicationEUI and ApplicationKey.
Can also be used as parameter for a Modem class

## Modems

### LoRa Modem
The *LoRaModem* class has one constructor with parameters like Hardware Serial and Credentials (see _Credentials_)
Another important function is setOption, with this function you can set LoRaOptions like on which port you want to send data, which spreadingfactor and if you want to send data acknowledged or not.
This class can also be used to get information from the modem, like status, version, datarate.

#### Setting Options
```
LoRaOptions options;
  
options.port = 1;
options.spreadingFactor = 7;
options.ack = false;

modem.setOptions(options);
```
*OR*
```
modem.setOptions(port); //will set port 
modem.setOptions(port, spreadingFactor); //will set port and spreadingfactor
modem.setOptions(port, ack); //will set port and acknowledgement
modem.setOptions(port, ack, spreadingFactor); will set port, acknowledgement and spreadingfactor
```
*OR you also can use following statements*
```
modem.setPort(1);
modem.setSpreadingFactor(9);
modem.setAck(false);
```


#### Retrieving Modem Configuration

```
modem.getModemVersion();
modem.getStatus();
modem.getSpreadingFactor();
```

The `LoRa Modem` class can be used as parameter in the Device class.

## Device
This class has only one parameter in it's constructor and that's a parameter of type Modem.
Class is used to send a payload to the modem. 
You can also send the options for the specific modem

```
device.send(&payload);
device.send(&payload, &options);
```

## Payload

### CBOR Payload
This class is almost the same as the previous version, except you have now one function to update assets.
```
CBORPayload payload;

payload.map(3);
payload.set("Battery", 39);
payload.set("DoorOpen", true);
payload.set("Temp", 24.5);
```

Like mentioned before, you use the device class to send the payload
```
device.send(&payload);
```

*CBOR payload also supports tag 103 and 120*

Tag 103 is automatically used when you add a Geolocation object (example GPSCBOR)
```
GeoLocation geoLocation(51.4546534, 4.127432, 11.2);
    
payload.map(1);
payload.set("loc", &geoLocation);

device.send(&payload);
```

Tag 120 is used when you set the timestamp (example iotDataPoint)
```
GeoLocation geoLocation(51.4546534, 4.127432, 11.2);
    
payload.map(4);
payload.set("bat", 98);
payload.set("title", "Hello Universe");
payload.set("loc", &geoLocation);
payload.set("doorClosed", true);
payload.setTimestamp(138914018);

device.send(&payload);
```

### Binary Payload
Like in CBOR, the set functionality is almost the same, except you have to translate the incoming data your self via ABCL.  Examples given.
```
BinaryPayload payload;

payload.set(17.56);
payload.set(true);
payload.set("hello");

device.send(&payload);
```
OR

```
BinaryPayload payload01("Hello");
BinaryPayload payload02("from ");

device.send(&payload01);
delay(5000);
device.send(&payload02);

```

# Actuation
You can also have actuation support in your sketch, the only thing you have to do is add following lines of code:
```
void callback(const unsigned char* payload, unsigned int length, uint8_t port );

OTAACredentials credentials(DEVEUI, APPEUI, APPKEY);
LoRaModem modem(&loraSerial, &debugSerial, &credentials, callback);
Device device(&modem);

void callback(const unsigned char* payload, unsigned int length, uint8_t port )
{
  for (int i = 0; i < length; i++)
  {
    debugSerial.print(payload[i], HEX);
  }
  debugSerial.println();
}
```

In the callback method you receive the payload that is sent from the backend.

**Parameters**
payload: the binary data received from the backend
length: total amounts of bytes in payload
port: on which port you received the data

# Examples

For all the examples in the SDK, please keep in mind that:

* You have an account on AllThingsTalkMaker
* Your device is created
* An asset '*counter*' of type integer is made; for the Geolocation example, also create asset 'loc' of type 'Location'
