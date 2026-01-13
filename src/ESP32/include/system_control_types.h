/**
 * @file SystemControl.h
 * @brief Defines the operational states for the turret system.
 */

#pragma once

#include <Arduino.h>

/**
 * @enum SystemControl
 * @brief Represents the primary control authority of the turret.
 */
enum class SystemControl : uint8_t
{
    USER_MODE = 0, /**< Manual override: Joystick inputs are mapped to motors. */
    AI_MODE   = 1  /**< Autonomous: Detection logic and Computer Vision drive motors. */
};

/**
 * @brief Helper utility to convert the SystemControl enum to a human-readable string.
 * @param mode The current system mode.
 * @return A descriptive string for logging.
 */
inline String modeToString(SystemControl mode) 
{
    switch (mode) 
    {
        case SystemControl::USER_MODE: return "USER_MANUAL";
        case SystemControl::AI_MODE:   return "AI_AUTONOMOUS";
    }
    return;
}

/**
 * @brief Toggles the system state between User and AI control.
 * @param currentMode The reference to the current system state.
 */
inline SystemControl toggleControlMode(SystemControl &currentMode)
{
    currentMode = (currentMode == SystemControl::USER_MODE) 
                  ? SystemControl::AI_MODE 
                  : SystemControl::USER_MODE;

    return currentMode;
}
