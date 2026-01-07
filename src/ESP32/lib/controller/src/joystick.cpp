#include "Joystick.h"

#include <Arduino.h>

void Joystick::begin()
{
    // Ensure ESP32 is using full 12-bit resolution
    analogReadResolution(12);
    
    pinMode(this->_pin_x, INPUT);
    pinMode(this->_pin_y, INPUT);
    
    // Auto-calibration: assume stick is at rest during startup
    this->_center_x = analogRead(this->_pin_x);
    this->_center_y = analogRead(this->_pin_y);
}

int Joystick::_read_axis(int pin, int center)
{
    int raw_val = analogRead(pin);
    int delta = raw_val - center;

    // Return 0 if the movement is within the noise floor (deadzone)
    if (abs(delta) < this->_deadzone)
    {
        return 0;
    }
    return delta;
}

const int Joystick::get_speed_x(int min_output, int max_output)
{
    int val = get_x();
    if (val == 0) { return 0; }

    // If positive: map from deadzone to max (4095 - center)
    // If negative: map from min (-center) to -deadzone
    if (val > 0)
    {
        return map(val, this->_deadzone, 4095 - this->_center_x, 0, max_output);
    }
    else
    {
        return map(val, -this->_center_x, -this->_deadzone, min_output, 0);
    }
}

const int Joystick::get_speed_y(int min_output, int max_output)
{
    int val = get_y();
    if (val == 0) { return 0; }

    if (val > 0)
    {
        return map(val, this->_deadzone, 4095 - this->_center_y, 0, max_output);
    }
    else
    {
        return map(val, -this->_center_y, -this->_deadzone, min_output, 0);
    }
}

bool Joystick::is_active()
{
    return (get_x() != 0 || get_y() != 0);
}