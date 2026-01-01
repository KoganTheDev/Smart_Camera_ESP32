#pragma once
#include <WiFi.h>

class Network
{

public:
    /**
     * @brief Initializes WiFi connection with a timeout.
     * @return true if connected, false if timeout reached.
     */
    static bool connect(String ssid, String password, uint32_t timeout_ms = 15000);

    /**
     * @brief Checks if WiFi is currently active.
     */
    static bool is_connected();

    /**
     * @brief Returns the local IP address as a string.
     */
    static String get_ip();
    
    /**
     * @brief Maintains connection. Call this occasionally if not using auto-reconnect.
     */
    static void handle_reconnect();
};