#include "Servo.h"
#include <Arduino.h>

namespace Turret
{
    Servo::Servo(uint8_t min_angle, uint8_t max_angle) 
    : _min_angle(min_angle), _max_angle(max_angle), 
      _current_angle(90), _target_angle(90), _speed(15), _last_move_time(0) {}

    void Servo::attach(int pin)
    {
        this->_servo.attach(pin);
        this->_servo.write(this->_current_angle); // Start at center
    }

    void Servo::set_target(int angle)
    {
        // Constrain the target to protect your hardware/wires
        this->_target_angle = constrain(angle, this->_min_angle, this->_max_angle);
    }

    void Servo::set_speed(int speed)
    {
        this->_speed = speed;
    }

    void Servo::set_position_raw(int angle)
    {
        this->_current_angle = constrain(angle, this->_min_angle, this->_max_angle);
        this->_target_angle = this->_current_angle;
        this->_servo.write(this->_current_angle);
    }

    void Servo::update()
    {
        // Check if it's time to take a step based on the speed setting
        if (millis() - this->_last_move_time >= (unsigned long)this->_speed)
        {
            if (this->_current_angle != this->_target_angle)
            {
                // Move one step toward the target
                if (this->_target_angle > this->_current_angle)
                {
                    this->_current_angle++;
                }
                else
                {
                    this->_current_angle--;
                }

                this->_servo.write(this->_current_angle);
                this->_last_move_time = millis();
            }
        }
    }
}
