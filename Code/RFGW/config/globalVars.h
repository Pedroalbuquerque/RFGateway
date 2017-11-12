// Json object
#include <ArduinoJson.h>

StaticJsonBuffer<200> jsonBuffer;
JsonObject& root = jsonBuffer.createObject();
uint16_t JsonLen = 0; // number of char loaded to radioJsonBuf

// buffer to support data tp be sent

char radioJsonBuf[MAX_JSON];

// flow & activity control

bool newDataFlag = false;

// node info and data storages
typedef struct{
  uint8_t nodeID;
  char topicName[50]; //not used for nodeCMD
  char topicValue[200];
  uint8_t complete =true;
  uint8_t newData = false;
} NODEINFO;

NODEINFO node[20];     // temporarely only nodes 0-19 are allowed
NODEINFO nodeCMD[20];  // node cmd received
#define MAX_NODEID 19 // for now restrict to 20
