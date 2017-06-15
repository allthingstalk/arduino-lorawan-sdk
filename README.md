arduinoLoRaWAN client library
=============================

This is a library by AllThingsTalk that provides connectivity to the cloud through [LoRa radios](https://www.lora-alliance.org/What-Is-LoRa/Technology).  

This library has been developed for the folowing LoRa modems: 

- [Microchip](http://www.microchip.com/wwwproducts/Devices.aspx?product=RN2483)
- others might follow

## Installation
  1. Download the [source code](https://github.com/allthingstalk/att-arduino-lorawan-client/archive/master.zip)
  2. copy the content of the zip file to your arduino libraries folder (usually found at &lt;arduinosketchfolder>/libraries) or import the zip through the arduino ide.

## features
The library has the following characteristics:
 
### async or sync
The Modem classes (LoRaModem and all descendents) have built in support to send data in a synchronous and asynchronous manner.  

- 'Send' will block untill the modem has responded that the transmission was succesfull (or not) and, if an ack was requested, untill the base station responded with the ack after succesfull transmission.
- 'SendAsync' will only block untill the modem has responded with a(n) (un)succesfull transmission, but will not wait for the ack, even if requested. This is handled by 'CheckSendState'.

### buffering
Buffering of data can occur in the following situations:

- When the modem reports a transmission failure. The system can  automatically retry to send this package.  
- if the application is sending data too fast.  LoRaWan devices can only use 1% of the radio space. Base stations can punish devices that go over this limit.  In order to prevent this, the system will calculate the minimum amount of time between 2 consecutive packages. If a message is sent too fast, it will instead be buffered and sent at the next available time-slot.  

Automatic data buffering is provided through the ATTDevice class. Whenever you use 'Send" from an instance of this class or from a [Container object](#data_formats), buffering is automatically activated.  
Buffered data is always handled asynchronically. Send will initiate the process while 'ProcessQueue' checks the ack responses (so that it knows when the next message can be sent).    

If you do not want to use any buffering, you can always use 'Send' or 'SendAsync' directly from the modem object.   

### data formats
The library allows you to send data in any format that you want. All it needs is a pointer to a memory location and the length of the byte stream.  
There are 2 pre-defined data structures included:

- *Containers*: this is the data format specific for [proximus](https://www.enco.io/) and used by the AllThingsTalk [RDK](http://shop.allthingstalk.com/product/lora-rapid-development-kit/) and [PDK](http://shop.allthingstalk.com/product/lorawan-track-and-trace/) products.
- *InstrumentationPacket*: this data structure can automatically collect and transmit a number of statistics regarding the modem. 

The predefined data structures always make use of the ATTDevice class for transmitting the data, so it is buffered and transmitted asynchronically.

---

For information on installing libraries, see: http://www.arduino.cc/en/Guide/Libraries

  
