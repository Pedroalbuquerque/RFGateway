// Json object
#include <ArduinoJson.h>

StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
uint16_t JsonLen = 0; // number of char loaded to radioJsonBuf

// buffer to support data tp be sent

char radioJsonBuf[MAX_JSON];

// flow & activity control

bool newDataFlag = false;
