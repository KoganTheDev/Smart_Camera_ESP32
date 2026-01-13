/**
 * @file BaseDetectionModule.h
 * @brief Abstract interface for object detection logic on ESP32-CAM.
 */

#pragma once

#include <esp_camera.h>
#include <move_types.h>
#include <tuple>

/**
 * @class BaseDetectionModule
 * @brief An abstract base class (Interface) that defines the contract for
 * detection algorithms. This class allows the Controller to remain agnostic of
 * the specific detection method (e.g., Color, AI, or Motion) by providing a
 * consistent interface.
 */
class BaseDetectionModule
{
public:
    /**
     * @brief Virtual destructor to ensure proper cleanup of derived classes.
     */
    virtual ~BaseDetectionModule() = default;

    /**
     * @brief Analyzes a camera frame to locate a specific object.
     * @param frame Pointer to the ESP32-CAM frame buffer (camera_fb_t).
     * @return std::tuple<uint8_t, uint8_t> Returns a tuple containing:
     * - Index 0 (MoveDirectionX): Left | Right | Stay
     * - Index 1 (MoveDirectionY): Up   | Down  | Stay
     */
    virtual std::tuple<MoveDirectionX, MoveDirectionY> detect_object(camera_fb_t* frame) = 0;
};
