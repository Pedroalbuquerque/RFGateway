#include <ESPBASE.h>


void WiFiconnect(){
  if(!WiFi.isConnected()){
    DEBUG_MSG("[WiFi] connecting to WiFi\n");

    uint8_t timeoutClick = 50;
    WiFi.begin(config.ssid.c_str(), config.password.c_str());
    while((WiFi.status()!= WL_CONNECTED) and --timeoutClick > 0) {
      delay(500);
      Serial.print(".");
    }
  }
  else
    DEBUG_MSG("[WiFi]WiFi already connected\n");

}



void WiFiSetup(){
  Esp.initialize();                // set environment and connect to Wifi router
  TelnetServer.begin();            // Optional in case you want to use telnet as Monitor
  TelnetServer.setNoDelay(true);   // Optional in case you want to use telnet as Monitor
  DEBUG_MSG("[WiFi] setup complete\n");
  WiFiconnect();
}

void WiFiloop(){
  ArduinoOTA.handle();
  server.handleClient();
  // activate telnet service to act as output console
  if (TelnetServer.hasClient()){            // Optional in case you want to use telnet as Monitor
      if (!Telnet || !Telnet.connected()){
        if(Telnet) Telnet.stop();
        Telnet = TelnetServer.available();
      } else {
        TelnetServer.available().stop();
      }
    }

  //  feed de DOG :)
  customWatchdog = millis();

}
