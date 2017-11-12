#include <Arduino.h>

#include <ESPBASE.h>
#include <RFM69.h>
#include <AsyncMqttClient.h>

// Load config
#include "config/all.h"

WiFiServer TelnetServer(23);  // Optional in case you want to use telnet as Monitor
WiFiClient Telnet;            // Optional in case you want to use telnet as Monitor

ESPBASE Esp; // object to support all WiFi services AP, OTA, Telnet, NTP, HTTP

void setup(){
  Serial.begin(115200);
  DEBUG_MSG("Booting...\t %lu\n", millis());
  mqttSetup();
  WiFiSetup();
  radioSetup();
  DEBUG_MSG("Complete \t %lu\n", millis());
}

void loop(){
  uint8_t tmpIndex;

  WiFiloop();
  radioLoop();
  mqttLoop();

  /*
  if((tmpIndex = radioLoop())){
    DEBUG_MSG("msg complete\t %s:%s\n",node[tmpIndex].topicName,node[tmpIndex].topicValue);
    mqttPublish(node[tmpIndex].topicName,node[tmpIndex].topicValue);
  }
  */

}
