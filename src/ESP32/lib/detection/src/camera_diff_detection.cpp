#include "camera_diff_detection.h"

#include <esp_camera.h>
#include <esp_heap_caps.h>

CameraDiffDetection::~CameraDiffDetection()
{
}


void CameraDiffDetection::roberts_cross(camera_fb_t* frame, uint8_t* output_edges)
{
    int w = frame->width;
    int h = frame->height;

    uint16_t * rgb_buf = (uint16_t *)heap_caps_malloc(w * h * 2, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);

    if (!rgb_buf) { return; }

    if (jpg2rgb565(frame->buf, frame->len, (uint8_t*)rgb_buf, JPG_SCALE_NONE))
    {
        for (int y = 0; y < h - 1; y++)
        {
            int curr_row = y * w;
            int next_row = (y + 1) * w;

            for (int x = 0; x < w - 1; x++)
            {
                uint8_t p1 = rgb565_to_greyscale(rgb_buf[curr_row + x]);
                uint8_t p2 = rgb565_to_greyscale(rgb_buf[curr_row + x + 1]);
                uint8_t p3 = rgb565_to_greyscale(rgb_buf[next_row + x]);
                uint8_t p4 = rgb565_to_greyscale(rgb_buf[next_row + x + 1]);

                int gx = p1 - p4;
                int gy = p2 - p3;

                // L1 Norm (Manhattan distance)
                int magnitude = abs(gx) + abs(gy);

                output_edges[curr_row + x] = (magnitude > 255) ? 255 : (uint8_t)magnitude;
            }
        }
    }

    // Ensure we free the memory regardless of success/failure
    free(rgb_buf);
}

uint8_t CameraDiffDetection::rgb565_to_greyscale(uint16_t pixel)
{
    // Extract R (5 bits), G (6 bits), G (5 bits) and move bits to a gretscale format
    uint32_t red = (pixel >> 11) & 0x1f;
    uint32_t green = (pixel >> 5) & 0x3f;
    uint32_t blue = pixel & 0x1f;

    // Approximation: shift to a [0,255] range
    return (uint8_t)((red * 2 + green * 5 + blue) << 1);
}


