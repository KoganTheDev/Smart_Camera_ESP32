#pragma once

#include <Arduino.h>

class MyStepper {
private:
    int _pins[4];
    int _current_pos;
    int _target_pos;
    int _step_phase; // Tracks the 0-3 sequence
    unsigned long _last_step_time;
    unsigned long _step_interval;

    void drive_pins(int phase); // Internal helper to toggle the 4 wires

public:
    MyStepper(int p1, int p2, int p3, int p4);
    
    void begin();
    void set_speed(int steps_per_second);
    void set_target(int relative_steps);
    void move_to(int absolute_position);
    void update();
    
    bool is_moving() { return _current_pos != _target_pos; }
};

