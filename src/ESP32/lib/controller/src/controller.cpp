#include "controller.h"

std::tuple<MoveDirectionX, MoveDirectionY> Controller::detect_object(std::tuple<uint8_t, uint8_t> current_location)
{
    // 1. Extract the raw numeric coordinates from the input
    uint8_t new_x = std::get<0>(current_location);
    uint8_t new_y = std::get<1>(current_location);

    // 2. Compare numbers (this->x) to numbers (new_x)
    // 3. Assign the result to an Enum (MoveDirectionX)
    MoveDirectionX x_axis_movement = (this->x > new_x ? MoveDirectionX::Left
        : (this->x < new_x) ? MoveDirectionX::Right
        : MoveDirectionX::None);

    MoveDirectionY y_axis_movement = (this->y > new_y ? MoveDirectionY::Down
        : (this->y < new_y) ? MoveDirectionY::Up
        : MoveDirectionY::None);
        
    return std::make_tuple(x_axis_movement, y_axis_movement);
}
