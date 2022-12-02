#include "client.hpp"
#include <WiFi.h>

// ~~~~~~~~~~~~~~~~~~~~~~~ Private Variable Declarations ~~~~~~~~~~~~~~~~~~~~~~

// Broker info
WiFiClient espClient;
PubSubClient mqtt_client(espClient);

// MQTT Client info
const char* clientID = "remotecar";
const char* willTopic = "car/status";
const uint8_t willQos = 1;
const boolean willRetain = true;
const char* willMessage = "offline";

// ~~~~~~~~~~~~~~~~~~~~~~~ Private Function Declarations ~~~~~~~~~~~~~~~~~~~~~~

void on_message(char *topic, byte *message, unsigned int len);

// ~~~~~~~~~~~~~~~~~~~~~~~ Public Function Definitions ~~~~~~~~~~~~~~~~~~~~~~~~

void client::init(void) {
    // Connect WiFi
    delay(10);
    // Connect to a WiFi network
    Serial.println("");
    Serial.print("Connecting to ");
    Serial.print(ssid);
    WiFi.begin(ssid, pass);

    // Wait for a successful connection...
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    // Announce a successful connection
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    // Start MQTT client
    mqtt_client.setServer(mqtt_broker_ip, mqtt_broker_port);
    mqtt_client.setCallback(on_message);

    // Connect client
    while (!mqtt_client.connected()) {
        reconnect();
    }
}

bool client::add_callback(char *topic, client::callback_t callback) {
    // TODO: this
    return false;
}

bool client::reconnect(void) {
    if (!mqtt_client.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqtt_client.connect(clientID, willTopic, willQos, willRetain, willMessage)) {
            Serial.println("connected");
        }
        else {
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
    return false;
}

bool client::publish(char *topic, byte *message, unsigned int len) {
    return mqtt_client.publish(topic, message, len);
}

// ~~~~~~~~~~~~~~~~~~~~~~~ Private Function Definitions ~~~~~~~~~~~~~~~~~~~~~~~

void on_message(char *topic, byte *message, unsigned int len){
    // Print info about message and parse into String
    Serial.print("Message arrived on topic: ");
    Serial.print(topic);
    Serial.print(". Message: ");
    String messageTemp;
    for (size_t i = 0; i < len; i++) {
        Serial.print((char)message[i]);
        messageTemp += (char)message[i];
    }
    Serial.println("");
}
