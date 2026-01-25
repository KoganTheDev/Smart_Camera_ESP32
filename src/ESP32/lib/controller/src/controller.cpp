#include "controller.h"
#include "camera.h"
#include <Arduino.h>

void Controller::run()
{
    if (this->_joystick.is_z_pressed()) // Flip control state
    {
        this->_system_control_state = toggleControlMode(this->_system_control_state);
        Serial.printf("[CONTROLLER] Mode changed to: %s\n", modeToString(this->_system_control_state).c_str());
    }

    std::tuple<MoveDirectionX, MoveDirectionY> move_directions;

    if (this->_system_control_state == SystemControl::USER_MODE) // USER mode
    {
        // USER_MODE: Read joystick and command motors
        int speed_x = this->_joystick.get_speed_x();
        int speed_y = this->_joystick.get_speed_y();

        MoveDirectionX user_yaw = (speed_x > 0)   ? MoveDirectionX::Right
                                  : (speed_x < 0) ? MoveDirectionX::Left
                                                  : MoveDirectionX::None;

        MoveDirectionY user_pitch = (speed_y > 0)   ? MoveDirectionY::Up
                                    : (speed_y < 0) ? MoveDirectionY::Down
                                                    : MoveDirectionY::None;

        move_directions = std::make_tuple(user_yaw, user_pitch);
    } else // AI mode
    {
        // AI_MODE: Capture frame, run detection, command motors
        camera_fb_t* fb = this->_camera.capture();

        if (fb)
        {
            // Run motion detection algorithm on the captured frame
            move_directions = this->_detection_module.detect_object(fb);
            MoveDirectionX move_x = std::get<0>(move_directions);
            MoveDirectionY move_y = std::get<1>(move_directions);

            // Log detection results only when motion is found
            if (move_x != MoveDirectionX::None || move_y != MoveDirectionY::None)
            {
                Serial.printf("[AI_MODE] Motion detected - Moving X:%s Y:%s\n", moveDirectionXToString(move_x), moveDirectionYToString(move_y));
            }

            // Release frame buffer
            this->_camera.release(fb);
        }
    }

    // Send the movement command to the movement manager
    this->_movement_manager.move_relative(move_directions);
}
