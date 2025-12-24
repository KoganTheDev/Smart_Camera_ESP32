#pragma once

#include <Arduino.h>

#include "network.h"
#include "constants.h"
#include "secrets.h"

void setup()
{
    Serial.begin(BAUDRATE);
    connect_to_WIFI(WIFI_SSID, WIFI_PASSWORD);
    
}

void loop()
{
    // put your main code here, to run repeatedly:
}
