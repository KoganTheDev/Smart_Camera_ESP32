#pragma once

#include "base_detection_module.h"

#include <stdlib.h>
#include <time.h>
#include <Arduino.h>

class TestDetection
{
private:
    /* data */
public:
    TestDetection() {}
    ~TestDetection() {}

    virtual std::tuple<uint8_t, uint8_t> detect_object(camera_fb_t* frame)
    {
        srand(time(0));

        int min = 0;
        int max = 2;

        uint8_t random_number_x = (rand() % (max - min + 1)) + min;
        uint8_t random_number_y = (rand() % (max - min + 1)) + min;

        Serial.print("x movement: ");
        switch (random_number_x)
        {
        case 0:
            Serial.println("stay");
            break;
        
        case 1:
            Serial.println("right");
            break;

        case 2:
            Serial.println("left");
            break;

        default:
            break;
        }

        
        Serial.print("y movement: ");
        switch (random_number_y)
        {
        case 0:
            Serial.println("stay");
            break;
        
        case 1:
            Serial.println("up");
            break;

        case 2:
            Serial.println("down");
            break;

        default:
            break;
        }


        Serial.printf("Tuple returned (%d, %d)", random_number_x, random_number_y);

        return std::make_tuple(random_number_x, random_number_y);
    };

};

