#include <AsyncMqttClient.h>
#include <Ticker.h>

AsyncMqttClient mqttClient;

Ticker mqttReconnectTimer;
Ticker wifiReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;

void mqttPublish(const char *topic, const char *payload){
  //mqttClient.publish(const char *topic, uint8_t qos, bool retain, optional const char *payload, optional size_t length)
  mqttClient.publish(topic, MQTT_QOS, MQTT_RETAIN, payload, strlen(payload));
  DEBUG_MSG("[mqtt]publish %s\tqos:%d\t payload:%s\n", topic, MQTT_QOS,payload);
}

void mqttSubscribe(const char *topic, uint8_t qos){
  mqttClient.subscribe(topic, qos);
  DEBUG_MSG("[mqtt]subscribe %s\tqos:%d\n", topic, qos);
}


void connectToMqtt() {
  DEBUG_MSG("[mqtt] Connecting to MQTT...");
  mqttClient.connect();
}


void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  DEBUG_MSG("[mqtt] Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  DEBUG_MSG("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, WiFiconnect); // when wifi connect, mqtt will connect also
}

void onMqttConnect(bool sessionPresent) {
  DEBUG_MSG("[mqtt] Connected to MQTT.\n");
  DEBUG_MSG("[mqtt] Session present: %d\n",sessionPresent);
  mqttClient.subscribe(MQTT_SUBS_MASK, 2);
  }

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  DEBUG_MSG("[mqtt]Disconnected from MQTT.\n");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  DEBUG_MSG("Subscribe acknowledged.");
  DEBUG_MSG("  packetId: %d\n",packetId);
  DEBUG_MSG("  qos: %d\n",qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  DEBUG_MSG("Unsubscribe acknowledged.");
  DEBUG_MSG("  packetId: %d\n",packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  uint8_t nodeID, start, end;
  String tmpStr;
  tmpStr = String(topic);
  end = tmpStr.lastIndexOf("/");
  start = tmpStr.lastIndexOf("/",end-1);
  nodeID = atoi(tmpStr.substring(start+1, end).c_str());

  //DEBUG_MSG("Publish received.\n");
  //DEBUG_MSG("  topic: %s\n", topic);
  //DEBUG_MSG("  NODE ID:%d\t star:%d\t end:%d \n",nodeID,start, end);
  //DEBUG_MSG("  qos: %d\n", properties.qos);
  //DEBUG_MSG("  dup: %d\n",properties.dup);
  //DEBUG_MSG("  retain: %d\n", properties.retain);
  //DEBUG_MSG("  len: %d\n", len);
  //DEBUG_MSG("  index: %d\n",index);
  //DEBUG_MSG("  total: %d\n",total);
  //DEBUG_MSG("msg:%s\n",payload);

  if(tmpStr.indexOf(MQTT_SUBTOPIC_CMD)!=-1){
    strcpy(nodeCMD[nodeID].topicValue,payload);
    nodeCMD[nodeID].nodeID = nodeID;
    nodeCMD[nodeID].complete = true;
    nodeCMD[nodeID].newData = true;
    radioSendNode(nodeID);
    DEBUG_MSG("[mqtt] CMD topic:%s\t value:%s\n\n",topic, payload);
  }
  //else
    //DEBUG_MSG("[mqtt] msg received but not CMD\n");
}

void onMqttPublish(uint16_t packetId) {
  //DEBUG_MSG("[mqtt] Publish acknowledged.\n");
  //DEBUG_MSG("[mqtt]  packetId: %d",packetId);
}

char* mqttBuildTopic(uint8_t nodeID, const char* subtopic){
  //char intStr[4]; // max 3 digit integer
  char tmpTopicC[50];
  String tmpTopic;
  tmpTopic = String(MQTT_TOPIC) + String(subtopic);
  tmpTopic.replace("{nodeid}",String(nodeID,DEC) );

  strcpy(tmpTopicC,tmpTopic.c_str());
  //DEBUG_MSG("[mqttbuild]%s\n",tmpTopicC);
  return tmpTopicC;
}


void mqttSetup() {

  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_SERVER, MQTT_PORT);


}

void mqttLoop() {

  for(uint8_t i =0; i< MAX_NODEID;i++){
    if(node[i].newData) {
      mqttPublish(node[i].topicName,node[i].topicValue);
      node[i].newData = false;
    }
  }
}
