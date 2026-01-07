/**
 * @file Servo.h
 * @brief Smooth-motion servo control for ESP32-CAM Turret axes.
 */

#pragma once

#include <ESP32Servo.h>

namespace Turret 
{
    /**
     * @class Servo
     * @brief Wrapper for the ESP32Servo library that adds acceleration/speed control.
     * This class prevents the "jerky" movement typical of standard servos by 
     * incrementing the position over time.
     */
    class Servo
    {
    private:
        ::Servo _servo;              ///< Underlying ESP32 PWM servo instance
        uint8_t _current_angle;      ///< Latest confirmed position (degrees)
        uint8_t _target_angle;       ///< Destination position (degrees)
        uint8_t _speed;              ///< Time delay between steps in milliseconds (ms/deg)
        uint8_t _min_angle;          ///< Lower safety limit for physical hardware
        uint8_t _max_angle;          ///< Upper safety limit for physical hardware
        unsigned long _last_move_time; ///< Timestamp (ms) of the last incremental step

    public:
        /**
         * @brief Construct a new Servo object.
         * @param min_angle The lower physical limit of the turret axis (default 0).
         * @param max_angle The upper physical limit of the turret axis (default 180).
         */
        Servo(uint8_t min_angle = 0, uint8_t max_angle = 180);

        /**
         * @brief Attaches the servo to a GPIO pin and initializes pulse settings.
         * @param pin The ESP32 GPIO number.
         */
        void attach(int pin);

        /**
         * @brief Sets a new destination angle for the servo to move toward.
         * @param angle The desired angle in degrees.
         * @details Clamped internally to [min_angle, max_angle].
         */
        void set_target(int angle);

        /**
         * @brief Manages incremental movement based on elapsed time.
         * @return true if a step was taken during this call, false otherwise.
         */
        void update();

        /**
         * @brief Bypasses speed control to move to a position instantly.
         * @param angle The angle to jump to.
         */
        void set_position_raw(int angle);

        /**
         * @brief Sets the interpolation speed.
         * @param speed Delay in milliseconds between each 1-degree increment. 
         * Higher = Slower (e.g., 20ms is smooth, 5ms is fast).
         */
        void set_speed(int speed);

        /**
         * @brief Returns the current estimated physical position.
         * @return uint8_t Current angle in degrees.
         */
        uint8_t get_current_angle() { return this->_current_angle; }
    };
};
