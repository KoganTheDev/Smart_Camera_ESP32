/**
 * @file Network.h
 * @brief Static utility for managing ESP32 WiFi connectivity.
 */

#pragma once

#include <WiFi.h>

/**
 * @class Network
 * @brief Provides a static interface for WiFi operations.
 */
class Network
{
public:
    /**
     * @brief Initializes WiFi connection in Station (STA) mode with a timeout.
     * @param ssid The SSID (name) of the WiFi network.
     * @param password The WiFi password.
     * @param timeout_ms Maximum time to wait for a connection in milliseconds. Default 15s.
     * @return true If the connection as successfully established.
     * @return false If the connection failed or timed out.
     */
    static bool connect(String ssid, String password, uint32_t timeout_ms = 15000);

    /**
     * @brief Checks the current hardware connection status.
     * @return true If the ESP32 is currently associated with an Access Point.
     */
    static bool is_connected();

    /**
     * @brief Retrieves the assigned local IP address.
     * @return String The formatted IP (e.g., "192.168.1.50") or "0.0.0.0" if disconnected.
     */
    static String get_ip();
};
