#include "wifi_manager.h"

#include "secrets.h"

void WifiManager::connect(const String& ssid, const String& password)
{
    if (ssid.length() == 0)
    {
        Serial.println("[WIFI] WiFi Error: SSID is empty");
        return;
    }

    Serial.printf("[WIFI] Initializing WiFi: %s\n", ssid.c_str());
    WiFi.mode(WIFI_STA); // Enter Station mode
    WiFi.begin(ssid, password);
    WiFi.setAutoReconnect(true);
}

bool WifiManager::is_connected() { return WiFi.status() == WL_CONNECTED; }

String WifiManager::get_ip() { return WiFi.localIP().toString(); }

void WifiManager::maintain()
{
    static unsigned long last_check = 0;
    static bool last_state = false;
    unsigned long now = millis();

    // Only check every 5 seconds to save CPU cycles
    if (now - last_check > 5000)
    {
        last_check = now;
        bool current_state = is_connected();

        if (current_state && !last_state)
        {
            Serial.printf("[WIFI] Connected! ESP's IP Address: %s\n", get_ip().c_str());
        } else if (!current_state)
        {
            if (last_state)
            {
                Serial.println("[WIFI] Network Lost. Waiting for auto-reconnect...");
            }
            WiFi.begin();
        }

        last_state = current_state;
    }
}
