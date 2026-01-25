#include "camera.h"

#include <Arduino.h>

bool Camera::begin()
{
    if (!psramFound())
    {
        Serial.println("ERROR: PSRAM not found!");
        return false;
    }

    Serial.printf("[CAMERA] PSRAM Free/Total Size: %d/%d bytes\n", ESP.getFreePsram(), ESP.getPsramSize());

    esp_err_t err = ESP_FAIL;

    err = esp_camera_init(&_config);

    if (err != ESP_OK)
    {
        Serial.println("[CAMERA] Initialization has failed");
        return false;
    }

    // Get sensor and configure for OV3660
    sensor_t* s = esp_camera_sensor_get();
    if (s == NULL)
    {
        Serial.println("ERROR: Failed to get sensor. Camera module not detected.");
        return false;
    }

    // Configure OV3660 specific settings for image clarity
    s->set_vflip(s, 1);      // Flip image upside-down
    s->set_brightness(s, 1); // Slight brightness boost reduces noise in low light
    s->set_contrast(s, 2);   // Boost contrast for clearer edges (better for motion detection)
    s->set_saturation(s, 1); // Slight saturation for color clarity

    return true;
}

camera_fb_t* Camera::capture() { return esp_camera_fb_get(); }

void Camera::release(camera_fb_t* fb) { esp_camera_fb_return(fb); }
