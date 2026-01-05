#include "network.h"
#include <Arduino.h> 
#include "secrets.h" 

bool Network::connect(String ssid, String password, uint32_t timeout_ms) {
    Serial.printf("Connecting to: %s\n", ssid.c_str());
    
    WiFi.mode(WIFI_STA); 
    WiFi.begin(ssid.c_str(), password.c_str()); 
    WiFi.setAutoReconnect(true); 

    uint32_t start_attempt_time = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - start_attempt_time < timeout_ms) {
        delay(500);
        Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println("\nWiFi Connected");
        Serial.printf("IP: %s\n", get_ip().c_str());
        return true;
    } else {
        Serial.println("\nWiFi Connection Failed (Timeout)");
        return false;
    }
}

bool Network::is_connected() {
    return WiFi.status() == WL_CONNECTED;
}

String Network::get_ip() {
    return WiFi.localIP().toString();
}
