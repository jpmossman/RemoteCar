#pragma once
#include <PubSubClient.h>

namespace client {
    typedef void (*callback_t)(char *topic, byte *message, unsigned int len);

    void init(void);
    bool add_callback(char *topic, callback_t callback);
    bool reconnect(void);
    bool publish(char *topic, byte *message, unsigned int len);
    // ? bool status(void);
    // ? loop

    // Network info
    static const char* ssid = "LAPTOP-1QLPB7T6 1976";
    static const char* pass = "1003P5<o6";

    // Broker info
    static const char* mqtt_broker_ip = "192.168.1.163";
    static const int mqtt_broker_port = 1883;
}
