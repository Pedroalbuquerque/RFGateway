/*

ESP69GW
MAIN MODULE

ESP8266 to RFM69 Gateway
Gateway code with suport for ESP8266-based boards

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <Arduino.h>
#include "config/all.h"

// -----------------------------------------------------------------------------
// Prototypes
// -----------------------------------------------------------------------------

#include <NtpClientLib.h>
#include <ESPAsyncWebServer.h>
#include <AsyncMqttClient.h>
#include "RFM69Manager.h"
#include "FS.h"
template<typename T> String getSetting(const String& key, T defaultValue);
template<typename T> bool setSetting(const String& key, T value);

struct _node_t {
  unsigned long count = 0;
  unsigned long missing = 0;
  unsigned long duplicates = 0;
  unsigned char lastPacketID = 0;
};

_node_t nodeInfo[255];

// -----------------------------------------------------------------------------
// METHODS
// -----------------------------------------------------------------------------

String getIdentifier() {
    char identifier[20];
    sprintf(identifier, "%s_%06X", DEVICE, ESP.getChipId());
    return String(identifier);
}

void ledOn() {
    digitalWrite(LED_PIN, LOW);
}

void ledOff() {
    digitalWrite(LED_PIN, HIGH);
}

void blink(unsigned int delayms, unsigned char times = 1) {
    for (unsigned char i=0; i<times; i++) {
        if (i>0) delay(delayms);
        ledOn();
        delay(delayms);
        ledOff();
    }
}

void clearCounts() {
    for(unsigned int i=0; i<255; i++) {
        nodeInfo[i].duplicates = 0;
        nodeInfo[i].missing = 0;
    }
}

void processMessage(packet_t * data) {

    blink(5, 1);

    DEBUG_MSG(
        "[MESSAGE] messageID:%d senderID:%d targetID:%d packetID:%d name:%s value:%s rssi:%d",
        data->messageID,
        data->senderID,
        data->targetID,
        data->packetID,
        data->name,
        data->value,
        data->rssi
    );

    // Detect duplicates and missing packets
    // packetID==0 means device is not sending packetID info
    if (data->packetID > 0) {
        if (nodeInfo[data->senderID].count > 0) {

            unsigned char gap = data->packetID - nodeInfo[data->senderID].lastPacketID;

            if (gap == 0) {
                DEBUG_MSG(" DUPLICATED");
                nodeInfo[data->senderID].duplicates = nodeInfo[data->senderID].duplicates + 1;
                return;
            }

            if ((gap > 1) && (data->packetID > 1)) {
                DEBUG_MSG(" MISSING PACKETS!!");
                nodeInfo[data->senderID].missing = nodeInfo[data->senderID].missing + gap - 1;
            }
        }

    }

    nodeInfo[data->senderID].lastPacketID = data->packetID;
    nodeInfo[data->senderID].count = nodeInfo[data->senderID].count + 1;

    DEBUG_MSG("\n");

    // Send info to websocket clients
    char buffer[60];
    sprintf_P(
        buffer,
        PSTR("{\"packet\": {\"senderID\": %u, \"targetID\": %u, \"packetID\": %u, \"name\": \"%s\", \"value\": \"%s\", \"rssi\": %d, \"duplicates\": %d, \"missing\": %d}}"),
        data->senderID, data->targetID, data->packetID, data->name, data->value, data->rssi,
        nodeInfo[data->senderID].duplicates , nodeInfo[data->senderID].missing);
    wsSend(buffer);

    // Try to find a matching mapping
    bool found = false;
    unsigned int count = getSetting("mappingCount", "0").toInt();
    for (unsigned int i=0; i<count; i++) {
        if ((getSetting("nodeid" + String(i)) == String(data->senderID)) &&
            (getSetting("key" + String(i)) == data->name)) {
            mqttSend((char *) getSetting("topic" + String(i)).c_str(), (char *) String(data->value).c_str());
            found = true;
            break;
        }
    }

    if (!found) {
        String topic = getSetting("defaultTopic", MQTT_DEFAULT_TOPIC);
        if (topic.length() > 0) {
            topic.replace("{nodeid}", String(data->senderID));
            topic.replace("{key}", String(data->name));
            mqttSend((char *) topic.c_str(), (char *) String(data->value).c_str());
        }
    }

}

// -----------------------------------------------------------------------------
// Hardware
// -----------------------------------------------------------------------------

void hardwareSetup() {
    Serial.begin(SERIAL_BAUDRATE);
    SPIFFS.begin();
    pinMode(LED_PIN, OUTPUT);
    ledOff();
}

void hardwareLoop() {

    // Heartbeat
    static unsigned long last_heartbeat = 0;
    if (mqttConnected()) {
        if ((millis() - last_heartbeat > HEARTBEAT_INTERVAL) || (last_heartbeat == 0)) {
            last_heartbeat = millis();
            mqttSend((char *) getSetting("hbTopic", MQTT_HEARTBEAT_TOPIC).c_str(), (char *) "1");
            DEBUG_MSG("[BEAT] Free heap: %d\n", ESP.getFreeHeap());
            DEBUG_MSG("[NTP] Time: %s\n", (char *) NTP.getTimeDateString().c_str());
        }
    }

}

void welcome() {

    delay(2000);
    Serial.printf("%s %s\n", (char *) APP_NAME, (char *) APP_VERSION);
    Serial.printf("%s\n%s\n\n", (char *) APP_AUTHOR, (char *) APP_WEBSITE);
    //Serial.printf("Device: %s\n", (char *) getIdentifier().c_str());
    Serial.printf("ChipID: %06X\n", ESP.getChipId());
    Serial.printf("Last reset reason: %s\n", (char *) ESP.getResetReason().c_str());
    Serial.printf("Memory size: %d bytes\n", ESP.getFlashChipSize());
    Serial.printf("Free heap: %d bytes\n", ESP.getFreeHeap());
    FSInfo fs_info;
    if (SPIFFS.info(fs_info)) {
        Serial.printf("File system total size: %d bytes\n", fs_info.totalBytes);
        Serial.printf("            used size : %d bytes\n", fs_info.usedBytes);
        Serial.printf("            block size: %d bytes\n", fs_info.blockSize);
        Serial.printf("            page size : %d bytes\n", fs_info.pageSize);
        Serial.printf("            max files : %d\n", fs_info.maxOpenFiles);
        Serial.printf("            max length: %d\n", fs_info.maxPathLength);
    }
    Serial.println();
    Serial.println();

}

// -----------------------------------------------------------------------------
// Bootstrap methods
// -----------------------------------------------------------------------------

void setup() {

    hardwareSetup();

    welcome();

    settingsSetup();
    if (getSetting("hostname").length() == 0) {
        setSetting("hostname", String() + getIdentifier());
        saveSettings();
    }

    wifiSetup();
    otaSetup();
    mqttSetup();
    radioSetup();
    webSetup();
    ntpSetup();

}

void loop() {

    hardwareLoop();
    settingsLoop();
    wifiLoop();
    otaLoop();
    mqttLoop();
    radioLoop();
    ntpLoop();

    yield();

}
