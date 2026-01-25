#include "camera_diff_detection.h"
#include "motion_data.h"

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

    // === STEP 1: Allocate buffers on first frame ===
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

    // === STEP 2: Convert current JPEG frame to greyscale ===
    if (!jpeg_to_greyscale(frame, _curr_frame))
    {
        Serial.printf("[DETECTION] ERROR: Frame decompression failed (Free heap: %u bytes)\n",
                      esp_get_free_heap_size());
        return std::make_tuple(MoveDirectionX::None, MoveDirectionY::None);
    }

    // === STEP 3: Skip first frame (need previous frame to compare) ===
    if (_first_frame)
    {
        memcpy(_prev_frame, _curr_frame, frame->width * frame->height);
        _first_frame = false;
        return std::make_tuple(MoveDirectionX::None, MoveDirectionY::None);
    }

    // === STEP 4: Detect motion by comparing frames ===
    int motion_centroid_x = 0, motion_centroid_y = 0;
    int motion_pixel_count = 0;

    bool motion_found = find_motion(_prev_frame, _curr_frame, frame->width, frame->height, motion_centroid_x,
                                    motion_centroid_y, motion_pixel_count);

    // === STEP 5: Save current frame as previous for next iteration ===
    memcpy(_prev_frame, _curr_frame, frame->width * frame->height);

    // === STEP 6: Store motion data for HTTP visualization ===
    // This happens FAST and doesn't allocate/deallocate memory
    if (motion_found)
    {
        _last_motion_data =
            MotionData(motion_centroid_x, motion_centroid_y, frame->width, frame->height, motion_pixel_count);
        // Debug output only every 30 frames to avoid serial bottleneck
        static int debug_counter = 0;
        if (++debug_counter >= 30)
        {
            Serial.printf("[DETECTION] Motion at (%d,%d), pixels: %d\n", motion_centroid_x, motion_centroid_y,
                          motion_pixel_count);
            debug_counter = 0;
        }
    } else
    {
        _last_motion_data = MotionData(); // Empty motion data (no motion)
    }

    if (!motion_found)
    {
        return std::make_tuple(MoveDirectionX::None, MoveDirectionY::None);
    }

    // === STEP 7: Convert motion centroid to movement direction ===
    int center_x = frame->width / 2;
    int center_y = frame->height / 2;

    MoveDirectionX x_dir = MoveDirectionX::None;
    MoveDirectionY y_dir = MoveDirectionY::None;

    // X-axis: determine if motion is left or right of center (with deadzone)
    if (motion_centroid_x < center_x - CENTER_DEADZONE)
    {
        x_dir = MoveDirectionX::Left;
    } else if (motion_centroid_x > center_x + CENTER_DEADZONE)
    {
        x_dir = MoveDirectionX::Right;
    }

    // Y-axis: determine if motion is up or down from center (with deadzone)
    if (motion_centroid_y < center_y - CENTER_DEADZONE)
    {
        y_dir = MoveDirectionY::Up;
    } else if (motion_centroid_y > center_y + CENTER_DEADZONE)
    {
        y_dir = MoveDirectionY::Down;
    }

    return std::make_tuple(x_dir, y_dir);
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

bool CameraDiffDetection::find_motion(uint8_t* prev, uint8_t* curr, int width, int height, int& center_x, int& center_y,
                                      int& pixel_count)
{
    // === Step 1: Analyze frame differences ===
    int total_diff = 0;    // Sum of all pixel differences
    int weighted_x = 0;    // Sum of (x * pixel_difference)
    int weighted_y = 0;    // Sum of (y * pixel_difference)
    int motion_pixels = 0; // Count of pixels above threshold

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int idx = y * width + x;
            int diff = abs((int)curr[idx] - (int)prev[idx]);

            // Store for visualization if needed
            _diff_buffer[idx] = (diff > 255) ? 255 : (uint8_t)diff;

            // Count pixels that changed significantly
            if (diff > DIFF_THRESHOLD)
            {
                weighted_x += x * diff;
                weighted_y += y * diff;
                total_diff += diff;
                motion_pixels++;
            }
        }
    }

    // === Step 2: Check if enough pixels changed ===
    if (motion_pixels < MOTION_THRESHOLD)
    {
        pixel_count = motion_pixels;
        return false; // Not enough motion
    }

    // === Step 3: Calculate weighted centroid ===
    center_x = weighted_x / total_diff;
    center_y = weighted_y / total_diff;

    // === Step 4: Filter sensor noise (reject motion at image edges) ===
    // These regions often have artifacts and false positives
    int bottom_edge_y = height - (height / 10); // Bottom 10%
    int top_edge_y = height / 20;               // Top 5%
    int left_edge_x = width / 20;               // Left 5%
    int right_edge_x = width - (width / 20);    // Right 5%

    // Only reject if motion is ONLY in edge region with insufficient pixels
    bool is_in_bottom_edge = (center_y > bottom_edge_y);
    bool is_in_top_edge = (center_y < top_edge_y);
    bool is_in_left_edge = (center_x < left_edge_x);
    bool is_in_right_edge = (center_x > right_edge_x);

    if ((is_in_bottom_edge || is_in_top_edge || is_in_left_edge || is_in_right_edge) &&
        motion_pixels < (MOTION_THRESHOLD * 2))
    {
        pixel_count = motion_pixels;
        return false; // Reject edge noise
    }

    // === Step 5: Motion confirmed ===
    pixel_count = motion_pixels;
    Serial.printf("[CAMERA] Motion detected - Centroid: (%d, %d), Pixels: %d\n", center_x, center_y, motion_pixels);
    return true;
}

bool CameraDiffDetection::jpeg_to_greyscale(camera_fb_t* frame, uint8_t* output)
{
    if (!frame || !output)
        return false;

    // === Step 1: Allocate RGB buffer for JPEG decompression ===
    int w = frame->width;
    int h = frame->height;
    size_t rgb_size = (size_t)w * h * 2;

    // Check available memory before allocation
    uint32_t free_heap = esp_get_free_heap_size();
    if (free_heap < (rgb_size + 10000)) // Need buffer + 10KB margin
    {
        Serial.printf("[CAMERA] WARNING: Low memory (Free: %u bytes, Need: %u)\n", free_heap, (unsigned int)rgb_size);
        return false;
    }

    uint16_t* rgb_buf = (uint16_t*)heap_caps_malloc(rgb_size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (!rgb_buf)
    {
        Serial.printf("[CAMERA] ERROR: Failed to allocate RGB buffer (Free: %u bytes, Need: %u)\n", free_heap,
                      (unsigned int)rgb_size);
        return false;
    }

    // === Step 2: Decompress JPEG to RGB565 ===
    bool decompress_ok = jpg2rgb565(frame->buf, frame->len, (uint8_t*)rgb_buf, JPG_SCALE_NONE);
    if (!decompress_ok)
    {
        Serial.println("[CAMERA] ERROR: JPEG decompression failed");
        heap_caps_free(rgb_buf);
        return false;
    }

    // === Step 3: Convert RGB565 to greyscale ===
    for (int i = 0; i < w * h; i++)
    {
        output[i] = rgb565_to_greyscale(rgb_buf[i]);
    }

    // === Step 4: Free temporary buffer immediately ===
    heap_caps_free(rgb_buf);
    rgb_buf = nullptr;
    return true;
}

MotionData CameraDiffDetection::get_motion_data() const { return _last_motion_data; }
