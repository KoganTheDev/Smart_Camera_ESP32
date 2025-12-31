#include "network.h"

void connect_to_WIFI(String ssid, String password)
{   
    Serial.printf("\nConnecting to: %s\nPassword: %s\n", WIFI_SSID, WIFI_PASSWORD);
    
    // Start WiFi connection
    WiFi.begin(ssid, password);

    // Wait for a connection to establish
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
    }

    Serial.println("WiFi Connected");
    Serial.printf("IP address: %s", WiFi.localIP().toString().c_str());
}
