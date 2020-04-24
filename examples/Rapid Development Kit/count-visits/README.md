# Count Visits Example

Visits counter example for AllThingsTalk LoRa SDK v2  
[**Please read the original guide here**](https://docs.allthingstalk.com/examples/kits/lorawan-rdk/sodaq-mbili-count-visits/)  
Uses a reed switch to check when the door opens, waits for the door to close and then increments the number of visits and uploads that data to AllThingsTalk. Also uses a momentary push button which, when clicked, resets the visits counter to zero and uploads that data as well.  
The asset name for the reed switch/door sensor is "15", which is the "Integer sensor" asset.  

## Usage

We'll use a SODAQ Mbili board for LoRa Communication, a reed switch (to detect a door opening) and a Grove Pushbutton (or any other pushbutton for that matter), connected as shown below:  

![Pushbutton Example Schema](schema.jpg?raw=true)

You'll need to change just a few minor things in the code so it fits your use case propery:

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
You might have noticed the *"keys.h"* present with the example. That's where the LoRa credentials are stored. 

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
If you didn't connect the reed switch and the pushbutton as shown in the image above, that's fine, you'll just need to change the numbers below to match the pin numbers to which the pushbutton and the reed switch are connected on your board:
```
int   reedPin             = [your reed switch pin number];
int   buttonPin           = [your pushbutton pin number];
```