/*     

      Home automation using Arduino and Xively v1.3.1
---------------------------------------------------------

Features in v1.3.1

-  Temperature sensing
-  Digital push button control for Air conditioners.
-  Beep when air conditioner is digitally activated. 
-  Two light control channels.

| Syby Abraham 2013 |
       
*/


#include <SPI.h>
#include <Ethernet.h>
#include <HttpClient.h>
#include <Xively.h>
#define LM35_PIN 4
#define ARDUINO_VOLTS 5000

int light1 = 6;
int light2 = 7;
int spkPin = 8;
int acPin = 9;

// MAC address for your Ethernet shield
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };

//Your Xively key
char xivelyKey[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

//Your Xively feed ID
#define xivelyFeed xxxxxxxxxx

// Define the strings for our datastream IDs
char sensorId[] = "Temperature";
char acID[] = "Airconditioner";
char light1ID[] = "Light 1"
char light2ID[] = "Light 2"
XivelyDatastream datastreams[] = 
{
  XivelyDatastream(sensorId, strlen(sensorId), DATASTREAM_FLOAT),
  XivelyDatastream(acID, strlen(acID), DATASTREAM_FLOAT),
  XivelyDatastream(light1ID, strlen(light1ID), DATASTREAM_CHAR),
  XivelyDatastream(light2ID, strlen(light2ID), DATASTREAM_FLOAT),
};
// Finally, wrap the datastreams into a feed
XivelyFeed feed(xivelyFeed, datastreams, 4 /* number of datastreams */);

EthernetClient client;
XivelyClient xivelyclient(client);

 

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(acPin, OUTPUT);
  pinMode(spkPin, OUTPUT);
  Serial.println("Starting multiple datastream upload to Xively...");
  Serial.println();

  while (Ethernet.begin(mac) != 1)
  {
    Serial.println("Error getting IP address via DHCP, trying again...");
    delay(15000);
  }
}

void loop() {
  
  //Pull AC state from Xively
  int getReturn = xivelyclient.get(feed, xivelyKey);    //get data from xively
  if(getReturn > 0)
  {
    Serial.println("AC Datastream");
    Serial.println(feed[1]);
  }
  else Serial.println("HTTP Error");
  
  //Switch on then off AC relay and beep once.

  if(feed[1].getFloat() >= 1)
  {
    digitalWrite(acPin, HIGH);
    digitalWrite(spkPin, HIGH);
    delay(150);
    digitalWrite(acPin, LOW);
    digitalWrite(spkPin, LOW);
  }
  
  //Convert LM35 reading to Celcius
  float val = analogRead(LM35_PIN);
  val = map(val, 0, 1023, 0, ARDUINO_VOLTS);
  float temp = (float)val / 10;
  
  //Send converted value to upload
  float sensorValue = float(temp);
  float acValue = 0;
  datastreams[0].setFloat(sensorValue);
  datastreams[1].setFloat(acValue);

  Serial.print("Read sensor value ");
  Serial.println(datastreams[0].getFloat());

  Serial.println("Uploading it to Xively");
  int ret = xivelyclient.put(feed, xivelyKey);
  Serial.print("xivelyclient.put returned ");
  Serial.println(ret);
  

 
  Serial.println();
  delay(5000);

}