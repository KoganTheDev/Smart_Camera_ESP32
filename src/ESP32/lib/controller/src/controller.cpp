#include "controller.h"

// debug
#include "constants.h"
//

void Controller::run() {
  if (this->_joystick.is_z_pressed()) // Flip control state
  {
    this->_system_control_state =
        toggleControlMode(this->_system_control_state);
    Serial.printf("[CONTROLLER] Mode changed to: %s\n",
                  modeToString(this->_system_control_state).c_str());
  }

  std::tuple<MoveDirectionX, MoveDirectionY> move_directions;

  if (this->_system_control_state == SystemControl::USER_MODE) // USER mode
  {
    // TODO: improve, maybe use the speed functionality of the joystick
    //? Currently mapped to the move directions instead of an actual speed

    int speed_x = this->_joystick.get_speed_x();
    int speed_y = this->_joystick.get_speed_y();

    MoveDirectionX user_yaw = (speed_x > 0)   ? MoveDirectionX::Right
                              : (speed_x < 0) ? MoveDirectionX::Left
                                              : MoveDirectionX::None;

    MoveDirectionY user_pitch = (speed_y > 0)   ? MoveDirectionY::Up
                                : (speed_y < 0) ? MoveDirectionY::Down
                                                : MoveDirectionY::None;

    //? TEMP FIX END

    move_directions = std::make_tuple(user_yaw, user_pitch);
  } else // AI mode
  {
    move_directions = this->_detection_module.detect_object(
        NULL); //! Used Null since it's' currently isn't attached to the camera
               //! logic
  }

  this->_movement_manager.move_relative(move_directions);
}
