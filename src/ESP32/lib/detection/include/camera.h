#pragma once

#include "esp_camera.h"

namespace WroverPins {
    #define PWDN_GPIO_NUM    -1 // When HIGH goes to sleep to save battery
    #define RESET_GPIO_NUM   -1 // Hard-reboot the camera if it gets "stuck"
    #define XCLK_GPIO_NUM    21 // External clock, used for communication between ESP32 and camera
    #define SIOD_GPIO_NUM    26 // I2C bus
    #define SIOC_GPIO_NUM    27 // SCCB bus
    #define Y9_GPIO_NUM      35 // data bus 7
    #define Y8_GPIO_NUM      34 // data bus 6
    #define Y7_GPIO_NUM      39 // data bus 5
    #define Y6_GPIO_NUM      36 // data bus 4
    #define Y5_GPIO_NUM      19 // data bus 3
    #define Y4_GPIO_NUM      18 // data bus 2
    #define Y3_GPIO_NUM       5 // data bus 1
    #define Y2_GPIO_NUM       4 // data bus 0
    #define VSYNC_GPIO_NUM   25 // VSYNC (Vertical sync) tells the ESP32 to reset the memory pointer to the start since a new image is coming
    #define HREF_GPIO_NUM    23 // HREF (Horizontal Reference), goes HIGH when a new line of pixels start
    #define PCLK_GPIO_NUM    22 // Pixel clock, used for communication between camera and ESP32
}

class Camera
{
private:
    camera_config_t _config;

public:
    Camera()
    {
        _config.ledc_channel = LEDC_CHANNEL_0;
        _config.ledc_timer = LEDC_TIMER_0;
        _config.pin_d0 = Y2_GPIO_NUM;
        _config.pin_d1 = Y3_GPIO_NUM;
        _config.pin_d2 = Y4_GPIO_NUM;
        _config.pin_d3 = Y5_GPIO_NUM;
        _config.pin_d4 = Y6_GPIO_NUM;
        _config.pin_d5 = Y7_GPIO_NUM;
        _config.pin_d6 = Y8_GPIO_NUM;
        _config.pin_d7 = Y9_GPIO_NUM;
        _config.pin_xclk = XCLK_GPIO_NUM;
        _config.pin_pclk = PCLK_GPIO_NUM;
        _config.pin_vsync = VSYNC_GPIO_NUM;
        _config.pin_href = HREF_GPIO_NUM;
        _config.pin_sccb_sda = SIOD_GPIO_NUM;
        _config.pin_sccb_scl = SIOC_GPIO_NUM;
        _config.pin_pwdn = PWDN_GPIO_NUM;
        _config.pin_reset = RESET_GPIO_NUM;
        _config.xclk_freq_hz = 20000000;
        _config.pixel_format = PIXFORMAT_JPEG; // Best for streaming to Laptop
        _config.frame_size = FRAMESIZE_QVGA;   // 320x240 - fast for AI
        _config.jpeg_quality = 12;
        _config.fb_count = 2;                  // Double buffering for FPS
        _config.fb_location = CAMERA_FB_IN_PSRAM;
        _config.grab_mode = CAMERA_GRAB_LATEST; // Always get the newest frame
    }

    bool begin();

    camera_fb_t* capture();
    void release(camera_fb_t* fb);
};
