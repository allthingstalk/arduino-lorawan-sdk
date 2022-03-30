#include <AllThingsTalk_LoRaWAN.h>
#include "keys.h"                                       // Load the header file that contains your credentials for LoRaWAN

#define debugSerial Serial                              // Define the serial interface that's going to be used for Serial monitor (debugging)
#define loraSerial Serial1                              // Define the serial interface that'll be used for communication with the LoRa module
#define PIRSensPin   A0                                 // Pin number to which our PIR sensor is connected to
#define debugSerialBaud 57600                           // Define the baud rate for the debugging serial port (used for Serial monitor)

bool pirValue;
bool previousPirValue;

ABPCredentials credentials(DEVADDR, APPSKEY, NWKSKEY);  // Define the credential variables loaded from the keys.h file (for ABP activation method)
LoRaModem modem(loraSerial, debugSerial, credentials);  // Define LoRa modem properties
CborPayload payload;

void setup() {
  // put your setup code here, to run once:
  debugSerial.begin(debugSerialBaud);                   // Initialize the debug serial port (for Serial monitor)

  while((!debugSerial) && (millis()) < 10000){}         // Wait for the Serial monitor to be open (so you can see all output) and if it isn't open in 10 seconds, run the sketch
  while (!modem.init()) { delay(1000); }                // Initialize the modem. If it fails, retry every 1 second until it succeeds
  
  pinMode(PIRSensPin, INPUT);                           // Read the status of our PIR sensor and save it in 'doorState' variable.
}

void loop() {
  // put your main code here, to run repeatedly:
  readSensors();
  sendSensorValues();
}

void readSensors() {                                    // Name of our function that we'll call from the loop function below
  delay(1000);
  
  pirValue = analogRead(PIRSensPin);                   // Read the data from the PIR Sensor pin and save it into the "pirValue" variable
  
  debugSerial.println("-----------------------");
  debugSerial.print("PIR Value: ");
  debugSerial.print(pirValue);
  debugSerial.println("");

  if (pirValue != previousPirValue) {
      if (pirValue) {
        debugSerial.println("Motion detected!");
      } else {
        debugSerial.println("Motion expired.");
      }
  
    sendSensorValues();
    previousPirValue = pirValue;
  }
}

void sendSensorValues() {                               // Function used to send the data we collected from all the sensors
  payload.reset();                                      // Reset the cbor payload (in case there's anything left from the previous one)
  payload.set("17", pirValue);                          // Create a payload containing data from the PIR Sensor as bool to be sent to asset name "17" (Created PIR sensor asser on AllThingsTalk Maker)
  modem.send(payload);                                  // Send all the data above to your AllThingsTalk account
}
