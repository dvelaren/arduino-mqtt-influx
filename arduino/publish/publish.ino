/* ESP32 MQTT publish example
*/

// Libraries
#include <WiFi.h>
#include <PubSubClient.h>
#include "DHT.h"
#include "secrets.h"

// I/O Labelling
#define DHTPIN 32

// Constants
#define DHTTYPE DHT11
#define TSEND 1000
const char* WIFI_SSID = SECRET_SSID;
const char* WIFI_PASSWORD = SECRET_PASS;
const char* MQTT_CLIENT_ID = "ESP32_your_client_id";
const char* MQTT_SERVER = "CHANGE_MQTT_SERVER";
const char* MQTT_USER = SECRET_MQTT_USER;
const char* MQTT_PASS = SECRET_MQTT_PASS;
const int MQTT_PORT = 1883;
const int MQTT_WS_PORT = 9001;
const char* MQTT_TOPIC = "CHANGE_MQTT_TOPIC";

// Variables
float t = 0;
float h = 0;
unsigned long timer = 0;
DHT dht(DHTPIN, DHTTYPE);
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Subroutines and functions
void wifiInit() {
  Serial.println("[WiFi] WiFi Init");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("[WiFi] Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
  WiFi.setAutoReconnect(true);
  Serial.print("[WiFi] IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("[MQTT] Attempting MQTT connection...");
    // Attempt to connect
    if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS)) {
      Serial.println(" connected");
      // Once connected, publish an announcement...
      String connectStr = String(MQTT_CLIENT_ID) + " connected";
      mqttClient.publish(MQTT_TOPIC, connectStr.c_str());
      // ... and resubscribe
      mqttClient.subscribe(MQTT_TOPIC);
    } else {
      Serial.print(" failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void readDHT() {
  float tact = dht.readTemperature();
  float hact = dht.readHumidity();

  if (isnan(tact) || isnan(hact)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  t = tact;
  h = hact;
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("[MQTT] Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void setup() {
  // Comms
  Serial.begin(115200);
  dht.begin();
  wifiInit();
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);
  mqttClient.setCallback(callback);
  timer = millis();
}

void loop() {
  if (!mqttClient.connected()) {
    reconnect();
  }
  if (millis() - timer >= TSEND) {
    readDHT();
    String payload = "{\"temp\":";
    payload += String(t);
    payload += ",\"hum\":";
    payload += String(h);
    payload += "}";
    Serial.println("[MQTT] Publish: " + payload);
    mqttClient.publish(MQTT_TOPIC, payload.c_str());
    timer = millis();
  }
  mqttClient.loop();
}