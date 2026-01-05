#include "stepper.h"

MyStepper::MyStepper(int p1, int p2, int p3, int p4) 
    : _current_pos(0), _target_pos(0), _step_phase(0), 
      _last_step_time(0), _step_interval(2000) {
    _pins[0] = p1; _pins[1] = p2; _pins[2] = p3; _pins[3] = p4;
}

void MyStepper::begin() {
    for(int i=0; i<4; i++) {
        pinMode(_pins[i], OUTPUT);
        digitalWrite(_pins[i], LOW);
    }
}

void MyStepper::drive_pins(int phase) {
    // Standard 4-step sequence for ULN2003
    const byte sequence[4] = { B1000, B0100, B0010, B0001 };
    
    for(int i=0; i<4; i++) {
        digitalWrite(_pins[i], bitRead(sequence[phase], 3-i));
    }
}

void MyStepper::set_speed(int steps_per_second) {
    if (steps_per_second > 0) {
        _step_interval = 1000000 / steps_per_second;
    }
}

void MyStepper::update() {
    if (_current_pos == _target_pos) {
        // Power down coils when not moving to prevent overheating
        for(int i=0; i<4; i++) digitalWrite(_pins[i], LOW);
        return;
    }

    unsigned long now = micros();
    if (now - _last_step_time >= _step_interval) {
        _last_step_time = now;

        if (_target_pos > _current_pos) {
            _current_pos++;
            _step_phase = (_step_phase + 1) % 4;
        } else {
            _current_pos--;
            _step_phase = (_step_phase <= 0) ? 3 : _step_phase - 1;
        }

        drive_pins(_step_phase);
    }
}

void MyStepper::set_target(int relative_steps) { _target_pos = _current_pos + relative_steps; }
void MyStepper::move_to(int absolute_position) { _target_pos = absolute_position; }