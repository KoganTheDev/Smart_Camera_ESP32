/**
 * @file Controller.h
 * @brief Header file for the Controller class.
*/
#pragma once

#include <stdlib.h>
#include <tuple>
#include <cstdint>
#include "move_types.h"

/**
 * @class Controller
 * @brief Handles logic for object detection and movement calculations.
 */
class Controller
{
private:
    uint8_t _x; /**< Internal x-coordinate state or threshold */
    uint8_t _y; /**< Internal y-coordinate state or threshold */

public:
    /**
     * @brief Construct a new Controller object.
     * @param x Initial x-coordinate or horizontal sensitivity.
     * @param y Initial y-coordinate or vertical sensitivity.
     */
    Controller(uint8_t x, uint8_t y) : _x(x), _y(y) {}

    /** @brief Destroy the Controller object. */
    ~Controller();

    /**
     * @brief Analyzes a location to determine required movement directions.
     * @param current_location A tuple containing the {x, y} coordinates of the detected object.
     * @return std::tuple<MoveDirectionX, MoveDirectionY> The calculated directions to move.
     */
    std::tuple<MoveDirectionX, MoveDirectionY> detect_object(std::tuple<uint8_t, uint8_t> current_location);
};
