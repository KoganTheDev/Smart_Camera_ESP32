#pragma once

#include <stdlib.h>
#include <tuple>


enum class MoveDirectionX : uint8_t { None = 0, Right = 1, Left = 2 };
enum class MoveDirectionY : uint8_t { None = 0, Up = 1, Down = 2 };

inline bool operator!(MoveDirectionX dir) {
    return dir == MoveDirectionX::None;
}


inline constexpr uint8_t operator+(MoveDirectionX dir) {
    return static_cast<uint8_t>(dir);
}


inline bool operator!(MoveDirectionY dir) {
    return dir == MoveDirectionY::None;
}

inline constexpr uint8_t operator+(MoveDirectionY dir) {
    return static_cast<uint8_t>(dir);
}


class Controller
{
private:
    uint8_t x;
    uint8_t y;
public:
    Controller(uint8_t x, uint8_t y) : x(x), y(y) {}
    ~Controller();

    std::tuple<MoveDirectionX, MoveDirectionY> detect_object(std::tuple<uint8_t, uint8_t> current_location);

};

