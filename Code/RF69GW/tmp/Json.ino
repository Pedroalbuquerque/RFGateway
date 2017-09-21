#include <ArduinoJson.h>



void JsonLoop( unsigned long interval){
  static uint32_t timer = 0;

  if(timer > millis()) timer= millis();
  if (timer == 0 || millis()-timer > interval) {
    // at regular <interval> read sensors and fill JsonBuffer
    DEBUG_MSG("%s\n",JsonBuild());
    newDataFlag = true;  //signal dta new data is available to be used/sent
    timer= millis();
  }
}
char* JsonBuild(){
    root[F(DHT_TEMPERATURE_TOPIC)] = getDHTTemperature();
    root[F(DHT_HUMIDITY_TOPIC)] = getDHTHumidity();

    JsonLen = root.printTo(radioJsonBuf);
    return radioJsonBuf;

}
