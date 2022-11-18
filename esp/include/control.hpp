#pragma once

namespace control {
    static unsigned int loop_interval = 100; // milliseconds

    void init(void);
    bool loop(float deltaT);
    void set_speed(signed char speed); // speed = [-100,100]
    void set_turn_angle(int angle); // Angle in degrees
    void set_follow_line(bool follow);
    void set_headlights(bool lights);
}
