#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecure.h>
#include <DHT.h>
#include <Adafruit_Sensor.h>

const char* ssid = "bjorn";
const char* password = "helloworld";

String timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";  // Default PT

#define BUTTON_PIN D1
#define DHT_PIN D2
#define DHT_TYPE DHT11

DHT dht(DHT_PIN, DHT_TYPE); // DHT Object

// Function prototypes
bool check_switch();
String read_time();
void transmit(float temp, float hum, String timeReceived);
void connectWiFi();
float read_sensor_1();
float read_sensor_2();
void selectTimeZone();

void setup() {
    Serial.begin(115200);
    pinMode(BUTTON_PIN, INPUT_PULLUP);  // Setup button
    connectWiFi();                      // Connect to WIFI
    dht.begin();                        // Initialize sensor

    selectTimeZone();                   // Time zone selection
}

void loop() {
    if (check_switch()) {
        String timeReceived = read_time();
        float temp = read_sensor_1(); // Read Temp
        float hum = read_sensor_2();  // Read Humidity
        transmit(temp, hum, timeReceived);  // Send Data
    }
    delay(5000);  // Delay
}

// Function to allow user to select time zone via USB serial terminal
void selectTimeZone() {
    Serial.println("Select Your Time Zone (Default is PT):");
    Serial.println("1) Eastern Time Zone (ET) - New York City, NY");
    Serial.println("2) Central Time Zone (CT) - Chicago, IL");
    Serial.println("3) Mountain Time Zone (MT) - Denver, CO");
    Serial.println("4) Pacific Time Zone (PT) - Los Angeles, CA");
    Serial.println("5) Alaska Time Zone (AKT) - Anchorage, AK");
    Serial.println("6) Hawaii-Aleutian Time Zone (HAT) - Honolulu, HI");
    Serial.println("7) Atlantic Time Zone (AT) - San Juan, Puerto Rico");

    long startMillis = millis();  // Start time
    while (millis() - startMillis < 10000) {  // Wait for 10 seconds for input
        if (Serial.available()) {
            char input = Serial.read();
            switch (input) {
                case '1':
                    timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/New_York";
                    Serial.println("Eastern Time Zone selected.");
                    return;
                case '2':
                    timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Chicago";
                    Serial.println("Central Time Zone selected.");
                    return;
                case '3':
                    timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Denver";
                    Serial.println("Mountain Time Zone selected.");
                    return;
                case '4':
                    timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";
                    Serial.println("Pacific Time Zone selected.");
                    return;
                case '5':
                    timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Anchorage";
                    Serial.println("Alaska Time Zone selected.");
                    return;
                case '6':
                    timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=Pacific/Honolulu";
                    Serial.println("Hawaii-Aleutian Time Zone selected.");
                    return;
                case '7':
                    timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Puerto_Rico";
                    Serial.println("Atlantic Time Zone selected.");
                    return;
                default:
                    Serial.println("Invalid selection. Defaulting to Pacific Time.");
                    timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";
                    return;
            }
        }
    }
    Serial.println("No input received. Defaulting to Pacific Time.");
    timeApiUrl = "https://timeapi.io/api/Time/current/zone?timeZone=America/Los_Angeles";
}

// Function to check the switch (button press)
bool check_switch() {
    return digitalRead(BUTTON_PIN) == LOW;
}

// Function to get the current time from the API
String read_time() {
    WiFiClientSecure wifiClient;
    wifiClient.setInsecure();

    HTTPClient http;
    
    http.begin(wifiClient, timeApiUrl);
    int httpCode = http.GET();

    if (httpCode > 0) {
        String payload = http.getString();
        Serial.println("Received Time Data: " + payload);

        // Parse JSON
        StaticJsonDocument<512> doc;
        DeserializationError error = deserializeJson(doc, payload);

        if (!error) {
            int year = doc["year"];
            int month = doc["month"];
            int day = doc["day"];
            int hour = doc["hour"];
            int minute = doc["minute"];
            int seconds = doc["seconds"];

            // Format time as YYYY-MM-DD%20HH:MM:SS
            char timeBuffer[30];
            sprintf(timeBuffer, "%04d-%02d-%02d%%20%02d:%02d:%02d", year, month, day, hour, minute, seconds);
            return String(timeBuffer);
        } else {
            Serial.println("JSON Parsing Failed");
        }
    } else {
        Serial.println("Error getting time data: " + String(httpCode));
    }

    http.end();
    return "Error";  // In case of an error
}

// Function to transmit data
void transmit(float temp, float hum, String timeReceived) {
    WiFiClientSecure client;
    HTTPClient http;
    client.setInsecure();

    String url = "https://bjornlavik.com/db_insert.php?nodeId=node_1&nodeTemp=" + String(temp) +
                 "&nodeHum=" + String(hum) + "&timeReceived=" + timeReceived;

    // Print URL
    Serial.println("Connecting to the following URL:");
    Serial.println(url);

    // Check for WiFi
    if (WiFi.status() == WL_CONNECTED) {
        http.begin(client, url);

        Serial.println("Sending GET request...");

        int httpCode = http.GET();  // GET request

        if (httpCode > 0) {
            Serial.print("HTTP Code: ");
            Serial.println(httpCode);
            String payload = http.getString();
            Serial.println("Response: " + payload);
        } else {
            Serial.print("Error on sending GET request: ");
            Serial.println(httpCode);
        }
        http.end();
    }
}

// Function to connect to WiFi
void connectWiFi() {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi");
}

// Function to read temperature from DHT11 sensor
float read_sensor_1() {
    float temp = dht.readTemperature();
    Serial.print("Temperature: ");
    Serial.println(temp);

    if (isnan(temp)) {
        Serial.println("Failed to read temperature from DHT sensor!");
        return -1; // Return an error value if read fails
    }
    return temp;  // Return the temperature value
}

// Function to read humidity from DHT11 sensor
float read_sensor_2() {
    float humidity = dht.readHumidity();  // Read humidity
    Serial.print("Humidity: ");
    Serial.println(humidity);

    if (isnan(humidity)) {
        Serial.println("Failed to read humidity from DHT sensor!");
        return -1; // Return an error value if read fails
    }
    return humidity;  // Return the humidity value
}
