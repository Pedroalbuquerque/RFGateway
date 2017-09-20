#include <ESPBASE.h>

ESPBASE Esp;

void WiFiSetup(){
  Esp.initialize();
}

void WiFiloop(){
  ArduinoOTA.handle();
  server.handleClient();

  //  feed de DOG :)
  customWatchdog = millis();

}
