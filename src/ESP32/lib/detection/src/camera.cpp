#include "camera.h"

#include <Arduino.h>

bool Camera::begin()
{
    esp_err_t err = esp_camera_init(&_config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed: 0x%x\n", err);
        return false;
    }

    sensor_t* s = esp_camera_sensor_get();
    s->set_vflip(s, 1); // flip image upside-down

    return true;
}

camera_fb_t* Camera::capture() { return esp_camera_fb_get(); }

void Camera::release(camera_fb_t* fb) { esp_camera_fb_return(fb); }
