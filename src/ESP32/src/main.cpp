#include <Arduino.h>
#include "constants.h"
#include "test_detection.h"
#include "joystick.h"
#include "movement_manager.h"
#include "controller.h"

Servo servo;
Stepper stepper(STEPPER_NUMBER_OF_STEPS, STEPPER_PIN1, STEPPER_PIN3, STEPPER_PIN2, STEPPER_PIN4);

MovementManager movement_manager(stepper, servo); 
TestDetection detection_manager;
Joystick joystick(JOYSTICK_PIN_X, JOYSTICK_PIN_Y);

Controller controller(movement_manager, detection_manager, joystick);

void setup()
{  
    Serial.begin(BAUDRATE);
    Serial.println("[Serial] === SMART TURRET START ===");


    servo.attach(SERVO_PIN);
    stepper.setSpeed(STEPPER_SPEED);
}

void loop()
{
    controller.run();
}
