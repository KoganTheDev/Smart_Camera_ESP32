#include "camera_diff_detection.h"
#include "constants.h"
#include "controller.h"
#include "joystick.h"
#include "movement_manager.h"
#include <Arduino.h>

#include "secrets.h"
#include "wifi_manager.h"

#include "camera.h"
#include "turret_server.h"

Servo servo;
Stepper stepper(STEPPER_NUMBER_OF_STEPS, STEPPER_PIN1, STEPPER_PIN3, STEPPER_PIN2, STEPPER_PIN4);

MovementManager movement_manager(stepper, servo);
CameraDiffDetection detection_manager;
Joystick joystick(JOYSTICK_PIN_X, JOYSTICK_PIN_Y, JOYSTICK_PIN_Z);
HttpServer http_server;
Camera camera;

Controller controller(movement_manager, detection_manager, joystick, camera);

void setup()
{
    Serial.begin(BAUDRATE);
    delay(100);

    Serial.println("\n\n[SYSTEM] === SMART TURRET START ===");
    Serial.println("[SYSTEM] Initializing components...");

    joystick.begin();
    Serial.println("[SYSTEM] Joystick initialized");

    if (camera.begin())
    {
        Serial.println("[SYSTEM] Camera initialized successfully");
    } else
    {
        Serial.println("[SYSTEM] ERROR: Camera initialization failed!");
    }

    Serial.println("[SYSTEM] Connecting to WiFi...");
    WifiManager::connect(WIFI_SSID, WIFI_PASSWORD);

    if (http_server.start(&camera))
    {
        Serial.println("[SYSTEM] HTTP Server started successfully");
    } else
    {
        Serial.println("[SYSTEM] ERROR: HTTP Server failed to start!");
    }

    servo.attach(SERVO_PIN);
    Serial.println("[SYSTEM] Servo attached");

    Serial.println("[SYSTEM] Setup complete - AI_MODE active");
    Serial.println("[SYSTEM] Waiting for motion...\n");
}

void loop()
{
    // Controller handles everything:
    // - In USER_MODE: reads joystick, commands motors
    // - In AI_MODE: captures frames, runs motion detection, commands motors
    controller.run();

    // Maintain WiFi connection
    WifiManager::maintain();
}
