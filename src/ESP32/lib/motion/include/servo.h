#pragma once

#include <ESP32Servo.h>

namespace Turret 
{

    class Servo
    {
    private:
        ::Servo _servo;
        uint8_t _current_angle;
        uint8_t _target_angle;
        uint8_t _speed;
        uint8_t _min_angle;
        uint8_t _max_angle;
        unsigned long _last_move_time;

    public:
        /**
         * @brief Construct a new Servo object.
         * @param min_angle The lower physical limit of the turret axis (default 0).
         * @param max_angle The upper physical limit of the turret axis (default 180).
         */
        Servo(uint8_t min_angle = 0, uint8_t max_angle = 180);

        /**
         * @brief Destroy the Servo object and safely detach
         * 
         */
        ~Servo() {}

        /**
         * @brief Attaches the servo to a GPIO pin and initializes pulse settings.
         * @param pin The ESP32 GPIO number (e.g., GPIO 2).
         * @note For ESP32-Wrover, ensure the pin does not conflict with Camera/PSRAM.
         */
        void attach(int pin);

        /**
        * @brief Sets a new destination angle for the servo to move toward.
        * @param angle The desired angle in degrees.
        * @details The input is automatically constrained between min_angle and max_angle.
        */
        void set_target(int angle);

        /**
        * @brief Manages incremental movement based on elapsed time.
        * @important Must be called in the main loop() to function.
        * @details Checks if the time interval defined by set_speed() has passed 
        * before moving the servo by 1 degree.
        */
        void update();

        /**
        * @brief Bypasses smooth movement to snap the servo to a position immediately.
        * @param angle The angle to jump to. Useful for setup or emergency stops.
        */
        void set_position_raw(int angle);

        /**
        * @brief Sets the speed of the servo movement.
        * @param speed The delay in milliseconds between each 1-degree step.
        * Lower values result in faster movement.
        */
        void set_speed(int speed);


        /**
        * @brief Returns the current physical position of the servo.
        * @return uint8_t Current angle in degrees (0-180).
        */
        uint8_t get_current_angle() { return this->_current_angle; }
    };

};