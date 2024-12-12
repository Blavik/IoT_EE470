#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// WiFi variables
const char* ssid = "bjorn";
const char* password = "helloworld";

// MQTT variables
const char* mqtt_server = "058b8034220a4c2fa62b0707ad9aca62.s1.eu.hivemq.cloud";
const char* publishTopic = "testtopic/temp/outTopic/unique331";
const char* switchTopic = "testtopic/temp/outTopic/unique332";
const char* subscribeTopic = "testtopic/temp/inTopic";
#define publishInterval 15

// Pin Definitions
#define BUILTIN_LED 2
#define SWITCH_PIN D1
#define POTENTIOMETER_PIN A0

// Global Variables
WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastPublishTime = 0;
unsigned long switchPressTime = 0;
bool switchPressed = false;

#define MSG_BUFFER_SIZE 50

// Function to connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

// Callback for MQTT messages
void callback(char* topic, byte* payload, int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  // Handle received messages
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn LED on
    Serial.println("LED ON");
  } else if ((char)payload[0] == '0') {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn LED off
    Serial.println("LED OFF");
  }
}

// Function to reconnect to MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);

    if (client.connect(clientId.c_str(), "bjorn", "espPass1")) {
      Serial.println("connected");
      client.subscribe(subscribeTopic);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  pinMode(SWITCH_PIN, INPUT);
  digitalWrite(BUILTIN_LED, HIGH);

  Serial.begin(9600);
  setup_wifi();
  espClient.setInsecure();
  client.setServer(mqtt_server, 8883);
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long now = millis();

  if (now - lastPublishTime > publishInterval * 1000) {
    lastPublishTime = now;
    int potValue = analogRead(POTENTIOMETER_PIN);
    char potMsg[MSG_BUFFER_SIZE];
    snprintf(potMsg, MSG_BUFFER_SIZE, "Potentiometer: %d", potValue);
    Serial.print("Publishing: ");
    Serial.println(potMsg);
    client.publish(publishTopic, potMsg, true);
  }

  int switchState = digitalRead(SWITCH_PIN);
  if (switchState == HIGH && !switchPressed) {
    switchPressed = true;
    switchPressTime = now;
    Serial.println("Switch pressed - publishing 1");
    client.publish(switchTopic, "1", true);
  }

  if (switchPressed && (now - switchPressTime > 5000)) {
    Serial.println("Publishing 0 after switch release");
    client.publish(switchTopic, "0", true);
    switchPressed = false;
  }
}
