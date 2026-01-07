/**
 * @file MoveTypes.h
 * @brief Global movement definitions and operator overloads for the Smart Camera project.
 * @details This header defines coordinate-agnostic movement enums used for motor control
 * and object tracking, including helper operators for type-safe logic.
 */
#pragma once

#include <stdlib.h>

/**
 * @enum MoveDirectionX
 * @brief Represents movement states along the horizontal axis.
 */
enum class MoveDirectionX : uint8_t { 
    None = 0,  /**< No movement */
    Right = 1, /**< Movement to the right */
    Left = 2   /**< Movement to the left */
};

/**
 * @enum MoveDirectionY
 * @brief Represents movement states along the vertical axis.
 */
enum class MoveDirectionY : uint8_t { 
    None = 0, /**< No movement */
    Up = 1,   /**< Movement upwards */
    Down = 2  /**< Movement downwards */
};

/** @brief Logical NOT operator for MoveDirectionX. Returns true if direction is None. */
inline bool operator!(MoveDirectionX dir) {
    return dir == MoveDirectionX::None;
}

/** @brief Unary plus operator to cast MoveDirectionX to its underlying uint8_t value. */
inline constexpr uint8_t operator+(MoveDirectionX dir) {
    return static_cast<uint8_t>(dir);
}

/** @brief Logical NOT operator for MoveDirectionY. Returns true if direction is None. */
inline bool operator!(MoveDirectionY dir) {
    return dir == MoveDirectionY::None;
}

/** @brief Unary plus operator to cast MoveDirectionY to its underlying uint8_t value. */
inline constexpr uint8_t operator+(MoveDirectionY dir) {
    return static_cast<uint8_t>(dir);
}
