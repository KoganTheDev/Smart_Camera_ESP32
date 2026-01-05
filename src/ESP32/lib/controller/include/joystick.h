#pragma once

// This header is used for the HW-504 joystick

#include <stdlib.h>

#define JOYSTICK_DEADZONE 150 // Used for joystick's sensitivity

/**
 * @class Joystick
 * @brief Handles 12-bit analog input from a 2-axis joystick with auto-calibration.
 */
class Joystick
{
private:
    uint8_t _pin_x, _pin_y;
    int _deadzone;
    int _center_x, _center_y;


    /**
     * @brief Internal helper to process raw ADC data against the center point.
     * @param pin The GPIO pin to read.
     * @param center The calibrated center value for that pin.
     * @return Offset value from center, or 0 if within deadzone.
     */
    int read_axis(int pin, int center);    

public:
    /**
     * @brief Construct a new Joystick object.
     * @param pin_x ADC1-capable GPIO for the X-axis.
     * @param pin_y ADC1-capable GPIO for the Y-axis.
     * @param deadzone Sensitivity threshold (default: 150).
     */
    Joystick(uint8_t pin_x, uint8_t pin_y, int deadzone = JOYSTICK_DEADZONE) :
    _pin_x(pin_x), _pin_y(pin_y), _deadzone(deadzone), _center_x(0), _center_y(0)  {};

    /**
     * @brief Initializes pins and performs auto-calibration of the center point.
     * @note Ensure the joystick is at rest (untouched) when calling this.
     */
    void begin();

    /** @return Current relative X-axis position (-2048 to 2048). */
    const int get_x() { return read_axis(this->_pin_x, this->_center_x); }

    /** @return Current relative Y-axis position (-2048 to 2048). */
    const int get_y() { return read_axis(this->_pin_y, this->_center_y); }

    /**
     * @brief Scales the current X deflection to a specific output range.
     * @param min_output Lower bound of the output (e.g., -255 for PWM).
     * @param max_output Upper bound of the output (e.g., 255 for PWM).
     * @return Scaled value proportional to stick deflection.
     */
    const int get_speed_x(int min_output = -255, int max_output = 255);

    /**
     * @brief Scales the current Y deflection to a specific output range.
     * @param min_output Lower bound of the output.
     * @param max_output Upper bound of the output.
     * @return Scaled value proportional to stick deflection.
     */
    const int get_speed_y(int min_output = -255, int max_output = 255);

    /**
     * @brief Checks if the joystick is currently being moved by the user.
     * @return true if deflection exceeds the deadzone, false otherwise.
     */
    bool is_active();
};
