#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

const char* ssid = "bjorn";
const char* password = "helloworld";

const char* ledStatusUrl = "https://www.bjornlavik.com/results.txt";  
const char* rgbValuesUrl = "https://www.bjornlavik.com/rgb_values.txt";

#define LED_PIN D4
#define RED_PIN D3
#define GREEN_PIN D2
#define BLUE_PIN D1

void connectWiFi();
void checkLEDStatus();
void updateRGB();

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    connectWiFi();
}

void loop() {
    checkLEDStatus();
    updateRGB();
    delay(20000);  // Check every 20 seconds
}

// Connect to WiFi
void connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

// Check LED status
void checkLEDStatus() {

    WiFiClientSecure wifiClient;
    wifiClient.setInsecure();
    HTTPClient http;

    if (WiFi.status() == WL_CONNECTED) {
        http.begin(wifiClient, ledStatusUrl);
        Serial.print("Connecting to URL: ");
        Serial.println(ledStatusUrl);

        int httpCode = http.GET();
        Serial.print("HTTP GET Status Code: ");
        Serial.println(httpCode);

        if (httpCode > 0) {
            String payload = http.getString();
            Serial.print("Received Payload: ");
            Serial.println(payload);

            if (payload == "LED_ON") {
                digitalWrite(LED_PIN, HIGH);
                Serial.println("LED turned ON");
            } else {
                digitalWrite(LED_PIN, LOW);
                Serial.println("LED turned OFF");
            }
        } else {
            Serial.print("Failed to fetch LED status, HTTP Code: ");
            Serial.println(httpCode);
        }
        http.end();
    }
}

// Update RGB values
void updateRGB() {

    WiFiClientSecure wifiClient;
    wifiClient.setInsecure();
    HTTPClient http;

    if (WiFi.status() == WL_CONNECTED) {
        http.begin(wifiClient, rgbValuesUrl);
        Serial.print("Connecting to URL: ");
        Serial.println(rgbValuesUrl);

        int httpCode = http.GET();
        Serial.print("HTTP GET Status Code: ");
        Serial.println(httpCode);

        if (httpCode > 0) {
            String payload = http.getString();
            Serial.print("Received Payload: ");
            Serial.println(payload);

            int redValue = payload.substring(0, payload.indexOf(',')).toInt();
            int greenValue = payload.substring(payload.indexOf(',') + 1, payload.lastIndexOf(',')).toInt();
            int blueValue = payload.substring(payload.lastIndexOf(',') + 1).toInt();

            analogWrite(RED_PIN, redValue);
            analogWrite(GREEN_PIN, greenValue);
            analogWrite(BLUE_PIN, blueValue);

            Serial.print("Red: ");
            Serial.print(redValue);
            Serial.print(", Green: ");
            Serial.print(greenValue);
            Serial.print(", Blue: ");
            Serial.println(blueValue);
        } else {
            Serial.print("Failed to fetch RGB values, HTTP Code: ");
            Serial.println(httpCode);
        }
        http.end();
    }
}
