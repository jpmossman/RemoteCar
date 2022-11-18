#pragma once

// Remember to check all updates against camera_pins.h!

namespace pins {
    static const int uss_trig_pin = 2;     // Arduino pin 2
    static const int uss_echo_pin = 4;     // Arduino pin 3

    static const int LedGreen = 12;       // Arduino pin 12
    static const int LedRed = 13;         // Arduino pin 13

    static const int LineL = 7;            // Left line sensor
    static const int LineC = 8;            // Center line sensor
    static const int LineR = 10;           // Right line sensor

    static const int DriveLF = 3;          // Drive left motor forwards
    static const int DriveLB = 5;
    static const int DriveRF = 6;
    static const int DriveRB = 11;
}
