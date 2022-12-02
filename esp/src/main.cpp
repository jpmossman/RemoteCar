#include <Arduino.h>
#include "client.hpp"
#include "control.hpp"
#include "app.hpp"

unsigned long int deltaT;

void setup() {
    // Init serial and internet connections
    Serial.begin(115200);
    client::init();

    // Init camera
    app_init_camera();

    // Init servos and controller data
    control::init();

    // Start the web server
    startServer();
    startStream();

    deltaT = millis();
    
}

void loop() {
    deltaT = millis() - deltaT;
    control::loop(deltaT);
    delay(50);
}
