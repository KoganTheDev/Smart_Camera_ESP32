/**
 * @file motion_data.h
 * @brief Encapsulates detected motion information in a clean, abstract way.
 */

#pragma once

#include <Arduino.h>

/**
 * @class MotionData
 * @brief Clean abstraction for motion detection results.
 * @details Separates motion detection logic from visualization.
 * Contains all information needed to draw a target overlay.
 */
class MotionData
{
private:
    bool _detected;     ///< Whether motion was detected
    int _centroid_x;    ///< X coordinate of motion center (0 if not detected)
    int _centroid_y;    ///< Y coordinate of motion center (0 if not detected)
    int _frame_width;   ///< Width of frame where motion was detected
    int _frame_height;  ///< Height of frame where motion was detected
    int _pixel_count;   ///< Number of pixels that changed

public:
    /**
     * @brief Constructor: Create empty motion data (no motion detected).
     */
    MotionData()
        : _detected(false), _centroid_x(0), _centroid_y(0), _frame_width(0), _frame_height(0), _pixel_count(0)
    {
    }

    /**
     * @brief Constructor: Create motion data with values.
     * @param centroid_x X coordinate of detected motion
     * @param centroid_y Y coordinate of detected motion
     * @param frame_width Width of the analyzed frame
     * @param frame_height Height of the analyzed frame
     * @param pixel_count Number of pixels that changed
     */
    MotionData(int centroid_x, int centroid_y, int frame_width, int frame_height, int pixel_count)
        : _detected(true), _centroid_x(centroid_x), _centroid_y(centroid_y), _frame_width(frame_width),
          _frame_height(frame_height), _pixel_count(pixel_count)
    {
    }

    /**
     * @brief Check if motion was detected.
     * @return true if motion detected, false otherwise
     */
    bool is_detected() const { return _detected; }

    /**
     * @brief Get motion centroid X coordinate.
     * @return X pixel coordinate (0 if no motion)
     */
    int get_centroid_x() const { return _centroid_x; }

    /**
     * @brief Get motion centroid Y coordinate.
     * @return Y pixel coordinate (0 if no motion)
     */
    int get_centroid_y() const { return _centroid_y; }

    /**
     * @brief Get frame width.
     * @return Width in pixels
     */
    int get_frame_width() const { return _frame_width; }

    /**
     * @brief Get frame height.
     * @return Height in pixels
     */
    int get_frame_height() const { return _frame_height; }

    /**
     * @brief Get number of pixels that changed.
     * @return Pixel count
     */
    int get_pixel_count() const { return _pixel_count; }
};
