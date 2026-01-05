#pragma once

#include <base_detection_module.h>

class CameraDiffDetection : BaseDetectionModule
{
public:
    CameraDiffDetection();
    ~CameraDiffDetection();

    std::tuple<uint8_t, uint8_t> detect_object(camera_fb_t* frame);

    // Take pixel and convert to greyscale pixel
    uint8_t rgb565_to_greyscale(uint16_t pixel);
    
    void roberts_cross(camera_fb_t* frame, uint8_t* output_edges);


};
