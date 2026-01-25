#include "camera.h"

#include <Arduino.h>

bool Camera::begin()
{
    if (!psramFound())
    {
        Serial.println("ERROR: PSRAM not found!");
        return false;
    }

    Serial.printf("PSRAM Total Size: %d bytes\n", ESP.getPsramSize());
    Serial.printf("PSRAM Free Size: %d bytes\n", ESP.getFreePsram());

    esp_err_t err = ESP_FAIL;
    int attempts = 0;

    // Retry camera initialization up to 3 times
    while (err != ESP_OK && attempts < 3)
    {
        attempts++;
        Serial.printf("Camera init attempt %d...\n", attempts);

        err = esp_camera_init(&_config);
        if (err != ESP_OK)
        {
            Serial.printf("Camera init failed with error code: 0x%x (%s)\n", err, esp_err_to_name(err));
            if (attempts < 3)
            {
                Serial.println("Retrying in 1 second...");
                delay(1000);
            }
        }
    }

    if (err != ESP_OK)
    {
        Serial.println("\nCamera initialization FAILED after 3 attempts!");
        Serial.println("Troubleshooting:");
        Serial.println("  1. Verify all GPIO pins are correctly configured");
        Serial.println("  2. Check for GPIO conflicts with stepper/joystick pins");
        Serial.println("  3. Ensure PSRAM is properly initialized");
        Serial.println("  4. OV3660 requires proper I2C communication on pins 26/27");
        Serial.println("  5. Check camera module is securely connected");
        Serial.println("  6. Try swapping camera cable to different socket");
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

    // Warm up the sensor with a few captures
    Serial.println("Warming up camera sensor...");
    for (int i = 0; i < 5; i++)
    {
        camera_fb_t* fb = esp_camera_fb_get();
        if (fb)
        {
            esp_camera_fb_return(fb);
            delay(50);
        }
    }

    Serial.println("✓ Camera initialized successfully (OV3660)");
    return true;
}

camera_fb_t* Camera::capture() { return esp_camera_fb_get(); }

void Camera::release(camera_fb_t* fb) { esp_camera_fb_return(fb); }
