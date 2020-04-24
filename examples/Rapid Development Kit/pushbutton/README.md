# Pushbutton Example

Pushbutton example for AllThingsTalk LoRa SDK v2  
[**Please read the original guide here**](https://docs.allthingstalk.com/examples/kits/lorawan-rdk/)  
When the button is pressed, a value "true" is sent to AllThingsTalk.  
The asset name for the button is "3", which is the "Push Button" asset.

## Usage

We'll use a SODAQ Mbili board for LoRa Communication as well as a Grove Pushbutton (or any other pushbutton for that matter), connected as shown below:  

![Pushbutton+Reed Switch Example Schema](schema.jpg?raw=true)

You need to change just a few minor things in the code so it fits your use case properly

## LoRa Activation Method
Depending on which method of device activation for LoRa you're using (ABP or OTAA), you'll need to make sure the proper line of code is present. 
 
#### ABP (Activation-By-Personalization)
This is the default activation method already present in the sketch, but here it is just in case:
```
ABPCredentials credentials(DEVADDR, APPSKEY, NWKSKEY);
```
#### OTAA (Over-The-Air-Activation)
If you're using OTAA, you'll want to change the line shown above to the following one:
```
OTAACredentials credentials(DEVEUI, APPEUI, APPKEY);
```

## LoRa Credentials
You might have notices the *"keys.h"* present with the example. That's where the LoRa credentials are stored. 

#### ABP Credentials
In case you used *ABP*, you'll need to change the following part of the *"keys.h"* file:
```
//ABP Credentials
uint8_t DEVADDR[4] = { 0x00, 0x00, 0x00, 0x00 };
uint8_t APPSKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
uint8_t NWKSKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
```
Where the *DEVADDR* is the Device Address, *APPSKEY* is the App Session Key, and *NWKSKEY* is the Network Session Key.  
Make sure you enter the credentials in the format shown above.

#### OTAA Credentials
In case you used *OTAA*, you'll change the other part of the *"keys.h"* file:
```
//OTAA Credentials
uint8_t DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
uint8_t APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
uint8_t APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }
````
Where the *DEVEUI* is the Device EUI, the *APPEUI* is the Application EUI, and the *APPKEY* is, well, the App Key.

## Pins
If you didn't connect the pushbutton as shown in the image above, that's fine, you'll just need to change the number below to match the pin number to which the pushbutton is connected on your board:
```
int   buttonPin           = [your pin number];
```