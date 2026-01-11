#include <Arduino.h>
#include "constants.h"
#include "test_detection.h"
#include "joystick.h"
#include "movement_manager.h"
#include "controller.h"

#include "wifi_manager.h"
#include "secrets.h"

#include "turret_server.h"
#include "camera.h"

Servo servo;
Stepper stepper(STEPPER_NUMBER_OF_STEPS, STEPPER_PIN1, STEPPER_PIN3, STEPPER_PIN2, STEPPER_PIN4);

MovementManager movement_manager(stepper, servo); 
TestDetection detection_manager;
Joystick joystick(JOYSTICK_PIN_X, JOYSTICK_PIN_Y, JOYSTICK_PIN_Z);

Controller controller(movement_manager, detection_manager, joystick);

HttpServer http_server;
static Camera camera;

void setup()
{  
    Serial.begin(BAUDRATE);
    Serial.println("[Serial] === SMART TURRET START ===");
    joystick.begin();
    //camera.begin();
    
    WifiManager::connect(WIFI_SSID, WIFI_PASSWORD);
    //http_server.start(&camera);

    servo.attach(SERVO_PIN);
    stepper.setSpeed(STEPPER_SPEED);
}

void loop()
{
    controller.run();
    WifiManager::maintain();
}
