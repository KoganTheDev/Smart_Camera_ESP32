#pragma once

#include "base_detection_module.h"

#include <stdlib.h>
#include <time.h>
#include <Arduino.h>

#include "move_types.h"

class TestDetection : public BaseDetectionModule
{
private:
    static inline int print_limit = 0;

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

        if (print_limit == 100)
        {
            print_limit = 0;

            Serial.print("x movement: ");
        switch (random_number_x)
        {
        case MoveDirectionX::None:
            Serial.println("stay");
            break;
        
        case MoveDirectionX::Right:
            Serial.println("right");
            break;

        case MoveDirectionX::Left:
            Serial.println("left");
            break;

        default:
            break;
        }

        
        Serial.print("y movement: ");
        switch (random_number_y)
        {
        case MoveDirectionY::None:
            Serial.println("stay");
            break;
        
        case MoveDirectionY::Up:
            Serial.println("up");
            break;

        case MoveDirectionY::Down:
            Serial.println("down");
            break;

        default:
            break;
        }

        Serial.printf("Tuple returned (%d, %d)\n", random_number_x, random_number_y);
        }
        

        print_limit++;

        return std::make_tuple(random_number_x, random_number_y);
    };
};

