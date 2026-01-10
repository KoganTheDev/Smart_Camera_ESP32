#include "controller.h"


void Controller::run()
{
    std::tuple<MoveDirectionX, MoveDirectionY> move_directions = this->_detection_module.detect_object(NULL); //! Used Null since it's' currently isn't attached to the camera logic
    this->_movement_manager.move_relative(move_directions);
}
