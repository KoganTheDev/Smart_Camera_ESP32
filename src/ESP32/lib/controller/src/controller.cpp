#include "controller.h"

std::tuple<MoveDirectionX, MoveDirectionY> Controller::detect_object(std::tuple<uint8_t, uint8_t> current_location)
{
    // 1. E_xtract the raw numeric coordinates from the input
    uint8_t new_x = std::get<0>(current_location);
    uint8_t new_y = std::get<1>(current_location);

    // 2. Compare numbers (this->_x) to numbers (new__x)
    // 3. Assign the result to an Enum (MoveDirection_x)
    MoveDirectionX x_axis_movement = (this->_x > new_x ? MoveDirectionX::Left
        : (this->_x < new_x) ? MoveDirectionX::Right
        : MoveDirectionX::None);

    MoveDirectionY y_axis_movement = (this->_y > new_y ? MoveDirectionY::Down
        : (this->_y < new_y) ? MoveDirectionY::Up
        : MoveDirectionY::None);
        
    return std::make_tuple(x_axis_movement, y_axis_movement);
}
