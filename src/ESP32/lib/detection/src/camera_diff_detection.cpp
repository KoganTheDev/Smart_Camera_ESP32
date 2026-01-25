#include "camera_diff_detection.h"

#include <cstring>
#include <esp_camera.h>
#include <esp_heap_caps.h>

CameraDiffDetection::CameraDiffDetection()
    : _prev_frame(nullptr), _curr_frame(nullptr), _diff_buffer(nullptr), _buffers_allocated(false), _first_frame(true)
{
}

CameraDiffDetection::~CameraDiffDetection()
{
    if (_buffers_allocated)
    {
        if (_prev_frame)
            heap_caps_free(_prev_frame);
        if (_curr_frame)
            heap_caps_free(_curr_frame);
        if (_diff_buffer)
            heap_caps_free(_diff_buffer);
    }
}

std::tuple<MoveDirectionX, MoveDirectionY> CameraDiffDetection::detect_object(camera_fb_t* frame)
{
    if (!frame)
    {
        return std::make_tuple(MoveDirectionX::None, MoveDirectionY::None);
    }

    // Allocate buffers on first frame
    if (!_buffers_allocated)
    {
        size_t buffer_size = frame->width * frame->height;

        _prev_frame = (uint8_t*)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        _curr_frame = (uint8_t*)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
        _diff_buffer = (uint8_t*)heap_caps_malloc(buffer_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

        if (!_prev_frame || !_curr_frame || !_diff_buffer)
        {
            Serial.println("[DETECTION] Failed to allocate frame buffers");
            return std::make_tuple(MoveDirectionX::None, MoveDirectionY::None);
        }

        _buffers_allocated = true;
        Serial.printf("[DETECTION] Frame buffers allocated: %d bytes each\n", (int)buffer_size);
    }

    // Decompress current frame to greyscale
    if (!jpeg_to_greyscale(frame, _curr_frame))
    {
        Serial.println("[DETECTION] Frame decompression failed");
        return std::make_tuple(MoveDirectionX::None, MoveDirectionY::None);
    }

    // On first frame, just store it and return no motion
    if (_first_frame)
    {
        memcpy(_prev_frame, _curr_frame, frame->width * frame->height);
        _first_frame = false;
        Serial.println("[DETECTION] First frame captured, ready for motion detection");
        return std::make_tuple(MoveDirectionX::None, MoveDirectionY::None);
    }

    // Find motion by comparing frames
    int motion_x = 0, motion_y = 0;
    bool motion_detected = find_motion(_prev_frame, _curr_frame, frame->width, frame->height, motion_x, motion_y);

    // Save current frame as previous for next iteration
    memcpy(_prev_frame, _curr_frame, frame->width * frame->height);

    if (!motion_detected)
    {
        return std::make_tuple(MoveDirectionX::None, MoveDirectionY::None);
    }

    // Calculate frame center
    int center_x = frame->width / 2;
    int center_y = frame->height / 2;

    // Determine movement direction based on motion offset from center
    MoveDirectionX x_dir = MoveDirectionX::None;
    MoveDirectionY y_dir = MoveDirectionY::None;

    // X-axis (horizontal) - deadzone prevents jitter
    if (motion_x < center_x - CENTER_DEADZONE)
    {
        x_dir = MoveDirectionX::Left;
    } else if (motion_x > center_x + CENTER_DEADZONE)
    {
        x_dir = MoveDirectionX::Right;
    }

    // Y-axis (vertical) - deadzone prevents jitter
    if (motion_y < center_y - CENTER_DEADZONE)
    {
        y_dir = MoveDirectionY::Up;
    } else if (motion_y > center_y + CENTER_DEADZONE)
    {
        y_dir = MoveDirectionY::Down;
    }

    return std::make_tuple(x_dir, y_dir);
}

bool CameraDiffDetection::find_motion(uint8_t* prev, uint8_t* curr, int width, int height, int& center_x, int& center_y)
{
    int total_diff = 0;
    int weighted_x = 0;
    int weighted_y = 0;
    int motion_pixels = 0;

    // Compare frames and find differences
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;

            // Calculate absolute difference
            int diff = abs((int)curr[idx] - (int)prev[idx]);

            // Store difference for potential visualization
            _diff_buffer[idx] = (diff > 255) ? 255 : (uint8_t)diff;

            // If difference is above threshold, it's motion
            if (diff > DIFF_THRESHOLD)
            {
                weighted_x += x * diff;
                weighted_y += y * diff;
                total_diff += diff;
                motion_pixels++;
            }
        }
    }

    // Require minimum motion pixels
    if (motion_pixels < MOTION_THRESHOLD)
    {
        return false;
    }

    // Calculate weighted centroid
    center_x = weighted_x / total_diff;
    center_y = weighted_y / total_diff;

    // NOISE FILTERING: Reject if centroid is in extreme edges (sensor artifacts)
    // Skip bottom 10% of image (common sensor noise region)
    int bottom_edge_y = height - (height / 10);

    // Skip top 5% (rare to have motion there)
    int top_edge_y = height / 20;

    // Skip left/right 5% (edge artifacts)
    int left_edge_x = width / 20;
    int right_edge_x = width - (width / 20);

    // If motion is only in sensor artifact zones, reject it
    if (center_y > bottom_edge_y && motion_pixels < (MOTION_THRESHOLD * 2))
    {
        Serial.printf("[DETECTION] Rejected bottom noise: y=%d (pixels=%d)\n", center_y, motion_pixels);
        return false;
    }

    if (center_y < top_edge_y && motion_pixels < (MOTION_THRESHOLD * 2))
    {
        Serial.printf("[DETECTION] Rejected top noise: y=%d (pixels=%d)\n", center_y, motion_pixels);
        return false;
    }

    if ((center_x < left_edge_x || center_x > right_edge_x) && motion_pixels < (MOTION_THRESHOLD * 2))
    {
        Serial.printf("[DETECTION] Rejected edge noise: x=%d (pixels=%d)\n", center_x, motion_pixels);
        return false;
    }

    return true;
}

uint8_t CameraDiffDetection::rgb565_to_greyscale(uint16_t pixel)
{
    // Extract R (5 bits), G (6 bits), B (5 bits) from RGB565 format
    uint8_t red = (pixel >> 11) & 0x1f;  // Red: 5 bits (0-31)
    uint8_t green = (pixel >> 5) & 0x3f; // Green: 6 bits (0-63)
    uint8_t blue = pixel & 0x1f;         // Blue: 5 bits (0-31)

    // Scale to 0-255 range and apply luminance formula
    // R: 0-31 → 0-255 (multiply by 255/31 ≈ 8.2, use 8 for speed)
    // G: 0-63 → 0-255 (multiply by 255/63 ≈ 4.0, use 4)
    // B: 0-31 → 0-255 (multiply by 255/31 ≈ 8.2, use 8 for speed)
    // Luminance: 0.299*R + 0.587*G + 0.114*B
    // Using integer approximation: (77*R + 150*G + 29*B) >> 8

    uint32_t r_scaled = red << 3;   // red * 8
    uint32_t g_scaled = green << 2; // green * 4
    uint32_t b_scaled = blue << 3;  // blue * 8

    uint32_t grey = (r_scaled * 77 + g_scaled * 150 + b_scaled * 29) >> 8;

    return (uint8_t)grey;
}
