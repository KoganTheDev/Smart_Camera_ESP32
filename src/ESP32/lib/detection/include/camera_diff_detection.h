#pragma once

#include <Arduino.h>
#include <base_detection_module.h>

/**
 * @class CameraDiffDetection
 * @brief Motion detection using frame differencing.
 * @details Detects motion by comparing consecutive frames and finding regions
 * where pixel values change significantly. This is much better than edge detection
 * for actual motion tracking.
 */
class CameraDiffDetection : public BaseDetectionModule
{
private:
    // Sensitivity parameters (tune these for your environment)
    // QVGA is 320x240 = 76,800 pixels total
    static const int DIFF_THRESHOLD = 20; ///< Pixel difference threshold (0-255) - lowered to catch finer motion
    static const int MOTION_THRESHOLD =
        800; ///< Minimum pixels that changed to consider motion - raised to reduce noise false positives
    static const int CENTER_DEADZONE =
        80; ///< Pixels around center where no movement command is sent - increased for stability

    uint8_t* _prev_frame;    ///< Previous frame greyscale buffer
    uint8_t* _curr_frame;    ///< Current frame greyscale buffer
    uint8_t* _diff_buffer;   ///< Difference map buffer
    bool _buffers_allocated; ///< Flag to track buffer allocation
    bool _first_frame;       ///< Flag for first frame (no previous frame)

public:
    CameraDiffDetection();
    ~CameraDiffDetection();

    /**
     * @brief Detects motion by comparing frames and returns direction to track.
     * @param frame Camera frame buffer
     * @return Tuple of (MoveDirectionX, MoveDirectionY) indicating motion direction
     */
    std::tuple<MoveDirectionX, MoveDirectionY> detect_object(camera_fb_t* frame) override;

private:
    /**
     * @brief Decompresses JPEG to greyscale frame.
     * @param frame JPEG camera frame
     * @param output Greyscale output buffer
     * @return true if decompression successful
     */
    bool jpeg_to_greyscale(camera_fb_t* frame, uint8_t* output);

    /**
     * @brief Computes frame difference and finds motion centroid.
     * @param prev Previous frame (greyscale)
     * @param curr Current frame (greyscale)
     * @param width Frame width
     * @param height Frame height
     * @param center_x Output center X coordinate
     * @param center_y Output center Y coordinate
     * @return true if significant motion was found
     */
    bool find_motion(uint8_t* prev, uint8_t* curr, int width, int height, int& center_x, int& center_y);

    /**
     * @brief Converts RGB565 pixel to greyscale.
     * @param pixel RGB565 formatted pixel
     * @return Greyscale value (0-255)
     */
    uint8_t rgb565_to_greyscale(uint16_t pixel);
};
