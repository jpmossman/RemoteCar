#include "sensors.hpp"
#include <Arduino.h>
#include "pins.hpp"

// ~~~~~~~~~~~~~~~~~~~~~~~ Private Variable Declarations ~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~ Private Function Declarations ~~~~~~~~~~~~~~~~~~~~~~

// ~~~~~~~~~~~~~~~~~~~~~~~ Public Function Definitions ~~~~~~~~~~~~~~~~~~~~~~~~

void sensors::init(void) {
    // Initialize line sensor pins as input
    pinMode(pins::LineL, INPUT);
    pinMode(pins::LineC, INPUT);
    pinMode(pins::LineR, INPUT);
    
    // Initialize distance sensors as input
    pinMode(pins::uss_echo_pin, INPUT);
    pinMode(pins::uss_trig_pin, INPUT);
}

unsigned int sensors::get_distance(void) {
    // TODO: This
    return 0;
}

sensors::LINE_STATUS sensors::get_line_status(bool black_on_white) {
    // Declare local variables for reading the sensor
    int left  = digitalRead (pins::LineL);
    int center  = digitalRead (pins::LineC);
    int right = digitalRead (pins::LineR);

    // Check for black on white or white on black
    // A high reading means black
    if (black_on_white) {
        // TODO: I dont think these conditions are perfectly correct
        if (left == right && right == center) return sensors::LINE_UNKNOWN;
        if (left  == right) return  sensors::LINE_ON;  // Line in the center
        if (left  == HIGH)  return  sensors::LINE_OFF_LEFT;  // Left on the line
        if (right == HIGH)  return  sensors::LINE_OFF_RIGHT;  // Right on the line
    } else {
        if (left == right && right == center) return sensors::LINE_UNKNOWN;
        if (left  == right) return  sensors::LINE_ON;  // Line in the center
        if (left  == LOW)   return  sensors::LINE_OFF_LEFT;  // Left on the line
        if (right == LOW)   return  sensors::LINE_OFF_RIGHT;  // Right on the line
    }

    return LINE_UNKNOWN;
}


// ~~~~~~~~~~~~~~~~~~~~~~~ Private Function Definitions ~~~~~~~~~~~~~~~~~~~~~~~
