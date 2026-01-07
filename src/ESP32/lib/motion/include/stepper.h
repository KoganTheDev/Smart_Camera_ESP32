/**
 * @file MyStepper.h
 * @brief Manual 4-wire Stepper Motor driver for precise Turret movement.
 * @details Implements a non-blocking step sequencer for 28BYJ-48 or similar motors.
 */

#pragma once

#include <Arduino.h>

/**
 * @class MyStepper
 * @brief Controls a 4-wire unipolar/bipolar stepper motor using a state-machine approach.
 * @details This class allows for simultaneous operation of other modules.
 * by avoiding delay() calls. It calculates pulse timing based on a microsecond interval.
 */
class MyStepper {
private:
    int _pins[4];               ///< GPIO array for Coil A, B, C, and D.
    int _current_pos;           ///< Tracked position in total steps since boot.
    int _target_pos;            ///< Destination position in absolute steps.
    int _step_phase;            ///< The current index (0-3 or 0-7) in the firing sequence.
    unsigned long _last_step_time; ///< Timestamp of the last physical step in microseconds.
    unsigned long _step_interval;  ///< Calculated delay (µs) between steps based on speed.

    /**
     * @brief Internal helper to set the high/low state of the 4 GPIO pins.
     * @param phase The current step in the sequence (0, 1, 2, 3).
     */
    void drive_pins(int phase); 

public:
    /**
     * @brief Construct a new My Stepper object.
     * @param p1 Coil 1 (IN1).
     * @param p2 Coil 2 (IN2).
     * @param p3 Coil 3 (IN3).
     * @param p4 Coil 4 (IN4).
     */
    MyStepper(int IN1, int IN2, int IN3, int IN4);
    
    /**
     * @brief Initializes GPIO pins as OUTPUT and sets initial state to LOW.
     */
    void begin();

    /**
     * @brief Sets the rotation velocity.
     * @param steps_per_second Frequency of steps. Higher = Faster rotation.
     */
    void set_speed(int steps_per_second);

    /**
     * @brief Sets a target destination relative to the current position.
     * @param relative_steps Number of steps to move (positive for CW, negative for CCW).
     */
    void set_target(int relative_steps) { this->_target_pos = this->_current_pos + relative_steps; }

    /**
     * @brief Sets an absolute target destination.
     * @param absolute_position The specific step count to reach.
     */
    void move_to(int absolute_position) { this->_target_pos = absolute_position; }

    /**
     * @brief Process engine. Calculates if it is time to take a step.
     * @important Must be called in the main loop() frequently.
     */
    void update();
    
    /**
     * @brief Checks if the motor is currently transitioning to a target.
     * @return true if the motor has not reached its destination.
     */
    bool is_moving() { return _current_pos != _target_pos; }
};
