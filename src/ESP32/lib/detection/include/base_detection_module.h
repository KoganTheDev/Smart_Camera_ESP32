#pragma once

#include <tuple>
#include <esp_camera.h>

class BaseDetectionModule
{
public:
    virtual std::tuple<uint8_t, uint8_t> detect_object(camera_fb_t* frame) = 0;
};
