#pragma once

#include <base_detection_module.h>

// TODO: Add DOCS

class CameraDiffDetection : public BaseDetectionModule
{
public:
    CameraDiffDetection();
    ~CameraDiffDetection();

    std::tuple<MoveDirectionX, MoveDirectionY> detect_object(camera_fb_t* frame) override;

    // Take pixel and convert to greyscale pixel
    uint8_t rgb565_to_greyscale(uint16_t pixel);

    void roberts_cross(camera_fb_t* frame, uint8_t* output_edges);
};
