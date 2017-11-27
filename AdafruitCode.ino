
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "NagRaj"
#define WLAN_PASS       "7060196036"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "NagRaj0308"
#define AIO_KEY         "9465a76ac80d42129e57e577dbd35055"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;


// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/



// Setup a feed called 'onoff' for subscribing to changes.

Adafruit_MQTT_Subscribe light1 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light1");

Adafruit_MQTT_Subscribe light2 = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/light2");
/*************************** Sketch Code ************************************/


void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);
  pinMode(D7,OUTPUT);
  pinMode(D1,OUTPUT);
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&light1);
  mqtt.subscribe(&light2);
}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &light1) {

      Serial.println((char *)light1.lastread);
      uint16_t num1 = atoi((char *)light1.lastread);
      digitalWrite(D1,!num1);
    }

    if (subscription == &light2) {
      
      Serial.println((char *)light2.lastread);
      uint16_t num2 = atoi((char *)light2.lastread);
      digitalWrite(D7,!num2);
    }
  }

 }

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
