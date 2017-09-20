#include <Arduino.h>

#include <ESPBASE.h>
#include <RFM69.h>

// Load config
#include "config/all.h"

void setup(){
  Serial.begin(115200);
  Serial.printf("Booting...\t %u\n", millis());
  WiFiSetup();
  radioSetup();
  Serial.printf("Complete \t %u\n", millis());
}

void loop(){
  //Serial.println("Alive!");
  WiFiloop();
  radioLoop();
}
