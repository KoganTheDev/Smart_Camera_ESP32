#pragma once

#include "base_detection_module.h"

#include <Arduino.h>
#include <stdlib.h>
#include <time.h>

#include "move_types.h"

class TestDetection : public BaseDetectionModule
{
public:
    TestDetection() {}
    ~TestDetection() {}

    std::tuple<MoveDirectionX, MoveDirectionY> detect_object(camera_fb_t* frame)
    {
        srand(time(0));

        int min = 0;
        int max = 2;

        MoveDirectionX random_number_x = MoveDirectionX((rand() % (max - min + 1)) + min);
        MoveDirectionY random_number_y = MoveDirectionY((rand() % (max - min + 1)) + min);

        return std::make_tuple(random_number_x, random_number_y);
    };
};
