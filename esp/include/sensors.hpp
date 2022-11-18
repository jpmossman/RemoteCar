#pragma once

namespace sensors {
    void init(void);
    unsigned int get_distance(void);
    enum LINE_STATUS {
        LINE_ON,
        LINE_OFF_LEFT,
        LINE_OFF_RIGHT,
        LINE_UNKNOWN
    };
    LINE_STATUS get_line_status(bool black_on_white);
}
