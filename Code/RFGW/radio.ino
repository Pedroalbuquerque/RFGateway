/*

RFM69 MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <RFM69.h>
#include <SPI.h>
#include <String.h>

RFM69 radio(SPI_CS, IRQ_PIN, IS_RFM69HW, digitalPinToInterrupt(IRQ_PIN));
//RFM69 radio;
/*
# ESP12    # RFM69CW
GPIO14	   SCK
GPIO12	   MISO
GPIO13	   MOSI
GPIO15	   NSS
GPIO4      DIO0
*/



#define MAX_CHARS 59
typedef struct{
  char msg[MAX_CHARS+1]; // 59 char+NULL to break message if necessary (61 max payload)
  uint8_t complete ; // 1 char to signal if msg contains full load
} Payload;

Payload data;


// -----------------------------------------------------------------------------
// RFM69
// -----------------------------------------------------------------------------

void radioSetup() {
    delay(10);
    radio.initialize(RADIO_FREQUENCY, RADIO_GATEWAYID, RADIO_NETWORKID);
    radio.setHighPower();
    radio.encrypt(RADIO_ENCRYPTKEY);
    radio.promiscuous(RADIO_PROMISCUOUS);

}


uint8_t radioLoop() {
  static unsigned long oldtimer;
  unsigned numBytes;
  uint8_t nodeIndex;

    if (radio.receiveDone()){//If some packet was received by the radio, wait for all its contents to come trough

      oldtimer = millis(); //For data link loss timeout calculation
      //DEBUG_MSG("Packet received \t #Bytes:%d\n", radio.PAYLOADLEN);
      if (radio.TARGETID == RADIO_GATEWAYID){//Check if the packet destination is the GATEWAY radio (NODE 1)
        if (radio.ACKRequested()){
          radio.sendACK();
        }
        nodeIndex = radio.SENDERID;

        //check if node info is complete, if so reset info to load newMode
        if(node[nodeIndex].complete){
          strcpy(node[nodeIndex].topicName,mqttBuildTopic(radio.SENDERID,MQTT_SUBTOPIC_STATUS));
          strcpy(node[nodeIndex].topicValue,"");
          node[nodeIndex].nodeID = radio.SENDERID;
          node[nodeIndex].complete = false;
          //DEBUG_MSG("[radio]init node topic:%s\tnodeID:%d\n",node[nodeIndex].topicName,node[nodeIndex].nodeID);
        }

        //Retrieve the data from the received Payload
        data = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else

        // Fill in node data with received radio packet until complete
        strcat(node[nodeIndex].topicValue,data.msg);
        node[nodeIndex].complete = data.complete;
        node[nodeIndex].newData = data.complete; //when full message receive mark to be sent to mqtt

        // blink to say "hi got a new packet!"
        blinkPin(LED,10);
      }
      else { // If no packets received
        if (millis() > (oldtimer + 5000)) { //if no packet received for the last 5 sec consider Link Lost
    		  Serial.println("missing packets!");
          oldtimer=millis();
          }
        }
      if(data.complete) return nodeIndex ;else return 0;
    }
    else
      return 0;
}

void radioSendNode( uint8_t nodeIndex){
    int bufLeft,numChars=0;
    uint8_t i =0;

    bufLeft = strlen(nodeCMD[nodeIndex].topicValue);

    //DEBUG_MSG("\n\n[radio]sending buflen:%d\n",bufLeft);
    //DEBUG_MSG("Node Index:%d\n",nodeIndex);
    //DEBUG_MSG("CMD value:%s\n",nodeCMD[nodeIndex].topicValue);
    //DEBUG_MSG("____________________\n\n");

    while( bufLeft > 0){
      numChars = bufLeft>MAX_CHARS? MAX_CHARS: bufLeft;
      strMid(data.msg,nodeCMD[nodeIndex].topicValue,i*MAX_CHARS,numChars);

      i++; // position on next 60 chars
      bufLeft -= MAX_CHARS ;
      if (bufLeft > 0) data.complete = false; else data.complete = true;

      if(!radio.sendWithRetry(nodeCMD[nodeIndex].nodeID, (const void*)&data, sizeof(data),RADIO_NUMRETRIES, RADIO_RETRYTIME)) {
        DEBUG_MSG("Error sending to Gateway:%d dataSize:%d\n\n",nodeCMD[nodeIndex].nodeID,sizeof(data));
        break;
      }

    } // while loop
    nodeCMD[nodeIndex].newData = false;  // mark nodeCMD as sent


}
