# Guard Your Stuff Example

Guard Your Stuff example for AllThingsTalk LoRa SDK v2  
[**Please read the original guide here**](https://docs.allthingstalk.com/examples/kits/lorawan-rdk/sodaq-mbili-guard-your-stuff/)  

Do you want to get notified when someone is moving things precious to you? This example shows how a LoRa device can be used to detect unexpected movement of an object and send out notifications to its owner. Furthermore, it shows how you can track the object using its GEO location.  
  
This example uses an Accelerometer module and a GPS Module. It uses the accelerometer module to check for small movement, and if detected, it sends GPS location to AllThingsTalk. Then it checks if there was a  significant movement between last and current GPS location checks. If there was, it'll continue sending GPS data to AllThingsTalk on defined intervals until GPS movement falls below threshold.  

This example uses:  
- SODAQ Mbili Board  
- LoRa module with antenna  
- Grove 3-Axis Accelerometer (NXP MMA7660)  
- Grove GPS v1.2 (SIM28)  

## Usage

We'll use a SODAQ Mbili board for LoRa Communication along with a Grove GPS and Accelerometer Sensor, connected as shown below:  

![Guard Your Stuff Example Schema](schema.png?raw=true)

You might need to change just a few minor things in the code so it fits your use case properly

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