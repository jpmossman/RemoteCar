#include "control.hpp"

// ~~~~~~~~~~~~~~~~~~~~~~~ Private Variable Declarations ~~~~~~~~~~~~~~~~~~~~~~

bool follow_line;
signed char drive_speed;
int drive_angle;

// ~~~~~~~~~~~~~~~~~~~~~~~ Private Function Declarations ~~~~~~~~~~~~~~~~~~~~~~

void update_motors();

// ~~~~~~~~~~~~~~~~~~~~~~~ Public Function Definitions ~~~~~~~~~~~~~~~~~~~~~~~~

void control::init(void) {
    follow_line = false;
}

bool control::loop(float deltaT) {
    if (follow_line) {
        // Adjust to follow line
    }
    return false;
}

// speed = [-100,100]
void control::set_speed(signed char speed) {
    drive_speed = speed;
    // clamp drive speed to be within [-100,100]
    drive_speed = drive_speed >  100 ?  100 : drive_speed;
    drive_speed = drive_speed < -100 ? -100 : drive_speed;
    update_motors();
}

// Angle in degrees
void control::set_turn_angle(int angle) {
    drive_angle = angle;
    update_motors();
}

void control::set_follow_line(bool follow) {
    follow_line = follow;
    update_motors();
}

void control::set_headlights(bool lights) {
    // TODO: this
}

// ~~~~~~~~~~~~~~~~~~~~~~~ Private Function Definitions ~~~~~~~~~~~~~~~~~~~~~~~

void update_motors() {
    // If follow line, adjust drive_speed and drive_angle
    if (follow_line) {

    }
    
    // Figure out LeftSpeed and RighSpeed based off of drive_speed and drive_angle
    // Set left  motor
    // analogWrite (DriveLF, (LeftSpeed > 0)  ? LeftSpeed     : 0);
    // analogWrite (DriveLB, (LeftSpeed < 0)  ? LeftSpeed*-1  : 0);
    // Set right motor
    // analogWrite (DriveRF, (RightSpeed > 0) ? RightSpeed    : 0);
    // analogWrite (DriveRB, (RightSpeed < 0) ? RightSpeed*-1 : 0);
}
