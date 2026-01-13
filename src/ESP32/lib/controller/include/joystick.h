/**
 * @file Joystick.h
 * @brief Driver for the HW-504 Analog 2-Axis Joystick + Integrated Button.
 */

#pragma once

#include <Arduino.h>

#define JOYSTICK_DEADZONE 150
#define JOYSTICK_RESOLUTION_BITS 12
#define JOYSTICK_SAMPLES 5    /**< Number of samples to average for noise reduction. */
#define BUTTON_DEBOUNCE_MS 50 /**< Debounce time for the Z-axis button. */

/**
 * @class Joystick
 * @brief Handles 12-bit analog input and debounced button state.
 */
class Joystick
{
private:
    uint8_t _pin_x, _pin_y, _pin_z;
    int _deadzone;
    int _center_x, _center_y;

    // Button state for Z-axis
    bool _last_btn_state;
    unsigned long _last_debounce_time;

    /** @brief Reads multiple ADC samples and returns the average. */
    int _read_raw(uint8_t pin);

    /** @brief Internal processing for axis deflection and deadzone filtering. */
    int _process_axis(int raw_val, int center) const;

    /** @brief Helper to map deflection to speed ranges. */
    int _map_speed(int val, int min_out, int max_out) const;

public:
    /**
     * @param pin_z GPIO for the joystick button.
     */
    Joystick(uint8_t pin_x, uint8_t pin_y, uint8_t pin_z, int deadzone = JOYSTICK_DEADZONE) : _pin_x(pin_x), _pin_y(pin_y), _pin_z(pin_z),
                                                                                              _deadzone(deadzone), _center_x(0), _center_y(0),
                                                                                              _last_btn_state(HIGH), _last_debounce_time(0) {};

    /**
     * @brief Initializes hardware pins and performs auto-calibration of the center point.
     * @details
     * 1. Sets X and Y pins to INPUT mode.
     * 2. Sets Z (button) pin to INPUT_PULLUP to prevent floating states.
     * 3. Configures the Global ADC resolution to 12-bit (0-4095).
     * 4. Calibrates the joystick by averaging the first 10 readings at rest.
     * @warning Ensure the joystick is untouched at the moment this is called,
     * as it defines the "zero" point for all future movement logic.
     */
    void begin();

    /** @return Relative deflection (-2048 to 2048). */
    int get_x() { return _process_axis(_read_raw(_pin_x), _center_x); }
    int get_y() { return _process_axis(_read_raw(_pin_y), _center_y); }

    /** @return true only on the moment the button is pressed (falling edge). */
    bool is_z_pressed();

    /** @return true if the button is currently held down. */
    bool is_z_held() const { return digitalRead(_pin_z) == LOW; }

    int get_speed_x(int min_out = -255, int max_out = 255) { return _map_speed(get_x(), min_out, max_out); }
    int get_speed_y(int min_out = -255, int max_out = 255) { return _map_speed(get_y(), min_out, max_out); }

    bool is_active();
};
