/*
 * MQTT and AskSensors IoT Platform
 * Description: ESP8266 sends NTP timestamps data to AskSensors using MQTT
 *  Author: https://asksensors.com, 2019
 *  github: https://github.com/asksensors
 */
 

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

//TODO: ESP8266 MQTT user config
const char* ssid = ".................."; // Wifi SSID
const char* password = ".................."; // Wifi Password
const char* username = "................."; // my AskSensors username
const char* pubTopic = "publish/..../....."; // publish/username/apiKeyIn
const unsigned int writeInterval = 25000;   // write interval (in ms)
//AskSensors MQTT config
const char* mqtt_server = "mqtt.asksensors.com";
unsigned int mqtt_port = 1883;

WiFiClient askClient; // WiFi client
PubSubClient client(askClient); // MQTT client

WiFiUDP ntpUDP;   // UDP client
NTPClient timeClient(ntpUDP); // NTP client

void setup() {
  Serial.begin(115200);
  Serial.println("*****************************************************");
  Serial.println("********** Program Start : ESP8266 publishes data to AskSensors over MQTT");
  Serial.print("********** connecting to WIFI : ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("->WiFi connected");
  Serial.println("->IP address: ");
  Serial.println(WiFi.localIP());
  
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback);

  timeClient.begin(); // init NTP
  timeClient.setTimeOffset(0); // 0= GMT, 3600 = GMT+1
}

void loop() {

  if (!client.connected()) 
    reconnect();
  client.loop();
  Serial.println("********** Publish MQTT data to ASKSENSORS");
  // timestamp
  while(!timeClient.update()) {
  timeClient.forceUpdate();
}
  // get Epoch time
  Serial.println(timeClient.getFormattedTime());
  long unsigned int timestamp = timeClient.getEpochTime();
  
  char mqtt_payload[100] = "";
  snprintf (mqtt_payload, 100, "module1=%ld&module2=%ld&t=%ld", random(10,100), random(10,100),timestamp);
  Serial.print("Publish message: ");
  Serial.println(mqtt_payload);
  client.publish(pubTopic, mqtt_payload);
  Serial.println("> MQTT data published");
  Serial.println("********** End ");
  Serial.println("*****************************************************");
  
 delay(writeInterval);// delay
}


void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("********** Attempting MQTT connection...");
    // Attempt to connect
if (client.connect("ESP8266Client", username, "")) { 
      Serial.println("-> MQTT client connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println("-> try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
