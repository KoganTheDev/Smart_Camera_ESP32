/**
 * @file main.cpp
 * @brief Entry point for the ESP32-CAM Turret Showcase.
 */

#include <Arduino.h>
#include "Camera.h"
#include "turret_server.h"
#include "network.h"
#include "secrets.h"

// Instantiate the core objects
Camera myCam;
HttpServer myServer;

void setup() {
    // 1. Start Serial for debugging telemetry
    Serial.begin(115200);
    Serial.println("\n--- ESP32-CAM Turret Showcase ---");

    // 2. Initialize the Camera hardware
    // Assuming your Camera class returns a bool on success
    if (!myCam.begin()) {
        Serial.println("CRITICAL ERROR: Camera initialization failed!");
        while (true) { delay(1000); } // Halt if no camera
    }
    Serial.println("Camera hardware initialized.");

    // 3. Connect to WiFi (Required for the Server)
    const char* ssid = WIFI_SSID;
    const char* pass = WIFI_PASSWORD;
    
    Serial.printf("Connecting to %s ", ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    
    Serial.println("\nWiFi Connected!");
    Serial.print("Access the Turret UI at: http://");
    Serial.println(WiFi.localIP());

    // 4. Start the Web Server
    // We pass the camera instance so the server can grab frames for the stream
    if (myServer.start(&myCam)) {
        Serial.println("Web Server started successfully.");
    } else {
        Serial.println("CRITICAL ERROR: Failed to start Web Server!");
    }
}

void loop() {
    // The HTTP Server runs in the background using ESP-IDF tasks,
    // so we don't need to put anything here for the server to work.
    
    // You can use the loop for low-priority background tasks
    // like checking battery levels or blinking a status LED.
    delay(1000);
}