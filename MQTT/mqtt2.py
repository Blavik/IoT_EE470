import paho.mqtt.client as mqtt
import ssl
import time
import requests

# MQTT Broker details
mqtt_broker = "058b8034220a4c2fa62b0707ad9aca62.s1.eu.hivemq.cloud"
mqtt_port = 8883
mqtt_username = "bjorn"
mqtt_password = "espPass1"
subscribe_topic = "testtopic/temp/outTopic/unique331"

php_url = "https://bjornlavik.com/insert_potentiometer.php"

# MQTT Client Setup
client = mqtt.Client()

# Set up credentials
client.username_pw_set(mqtt_username, mqtt_password)

client.tls_set_context(ssl.create_default_context())

# Connect to the MQTT Broker
def on_connect(client, userdata, flags, rc):
    print(f"Connected with result code {rc}")
    if rc == 0:
        client.subscribe(subscribe_topic)
    else:
        print(f"Failed to connect, return code {rc}")

client.on_connect = on_connect

# Callback for receiving messages
def on_message(client, userdata, msg):
    print(f"Message received on topic {msg.topic}: {msg.payload.decode()}")
    
    if "Potentiometer:" in msg.payload.decode():
        # Extract the ADC value (assuming the message format is like "Potentiometer: 1005")
        pot_value_str = msg.payload.decode().split(":")[1].strip()
        
        try:
            pot_value = int(pot_value_str)  # Convert the value to an integer
            print(f"Extracted potentiometer value: {pot_value}")

            try:
                response = requests.get(php_url, params={'adc_value': pot_value})
                if response.status_code == 200:
                    print(f"Potentiometer value {pot_value} sent to database successfully!")
                else:
                    print(f"Error sending data to PHP script: {response.status_code}")
            except requests.exceptions.RequestException as e:
                print(f"Error sending data to PHP script: {e}")
        
        except ValueError:
            print("Error: The potentiometer value could not be extracted as an integer.")
    else:
        print("Message does not contain potentiometer data, ignoring...")

client.on_message = on_message

# Connect to MQTT broker
client.connect(mqtt_broker, mqtt_port, 60)

# Start the loop to handle network traffic
client.loop_start()

# Keep the script running
try:
    while True:
        time.sleep(10)

except KeyboardInterrupt:
    print("Disconnected")
    client.loop_stop()
