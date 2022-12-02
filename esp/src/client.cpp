#include "client.hpp"
#include <WiFi.h>
#include <vector>
#include <string>
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

// Callback
std::vector<client::callback_t> calls;
std::vector<std::string> topics;

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
}

void client::add_callback(const char *topic, client::callback_t callback) {
    calls.push_back(callback);
    topics.push_back(topic);
}

bool client::topic_matches_sub(const char *topic, const char *sub) {
    return true;
}

bool client::reconnect(void) {
    // TODO: Fix
    if (!mqtt_client.connected()) {
        reconnect();
    }
    return false;
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

    for(size_t i = 0; i < calls.size(); i++) {
        if (topic_matches_sub(topic,topics[i])) {
            calls[i](topic,message,len);
        }
    }
}
