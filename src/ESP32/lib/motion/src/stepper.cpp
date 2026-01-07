#include "stepper.h"

MyStepper::MyStepper(int IN1, int IN2, int IN3, int IN4) 
    : _current_pos(0), _target_pos(0), _step_phase(0), 
      _last_step_time(0), _step_interval(2000)
    {
        _pins[0] = IN1; _pins[1] = IN2; _pins[2] = IN3; _pins[3] = IN4;
    }

void MyStepper::begin() {
    for (int i = 0; i < 4; i++)
    {
        pinMode(this->_pins[i], OUTPUT);
        digitalWrite(this->_pins[i], LOW);
    }
}

void MyStepper::drive_pins(int phase) {
    // Standard 4-step sequence for ULN2003
    const byte sequence[4] = { B1000, B0100, B0010, B0001 };
    
    for (int i = 0; i < 4; i++)
    {
        digitalWrite(this->_pins[i], bitRead(sequence[phase], 3-i));
    }
}

void MyStepper::set_speed(int steps_per_second) {
    if (steps_per_second > 0)
    {
        this->_step_interval = 1000000 / steps_per_second;
    }
}

void MyStepper::update()
{
    if (this->_current_pos == this->_target_pos)
    {
        // Power down coils when not moving to prevent overheating
        for (int i = 0; i < 4; i++)
        {
            digitalWrite(_pins[i], LOW);
        }
        return;
    }

    unsigned long now = micros();
    if (now - this->_last_step_time >= this->_step_interval) {
        this->_last_step_time = now;

        if (this->_target_pos > this->_current_pos)
        {
            this->_current_pos++;
            this->_step_phase = (this->_step_phase + 1) % 4;
        }
        else
        {
            this->_current_pos--;
            this->_step_phase = (this->_step_phase <= 0) ? 3 : this->_step_phase - 1;
        }

        drive_pins(this->_step_phase);
    }
}
