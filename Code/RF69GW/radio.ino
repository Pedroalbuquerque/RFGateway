/*

RFM69 MODULE

Copyright (C) 2016-2017 by Xose Pérez <xose dot perez at gmail dot com>

*/

#include <RFM69.h>
#include <SPI.h>

RFM69 radio(SPI_CS, IRQ_PIN, IS_RFM69HW, digitalPinToInterrupt(IRQ_PIN));
/*
# ESP12    # RFM69CW
GPIO14	   SCK
GPIO12	   MISO
GPIO13	   MOSI
GPIO15	   NSS
GPIO4      DIO0
*/


// Define the data packet struct that will be received from the onboard transmitter
typedef struct
{
  int16_t altitude; // int (2Byte)
  int16_t velocidade;
  int32_t latitude; //double (4)
  int32_t latitudedeg;
  int32_t longitude;
  int32_t longitudedeg;
  char latlon[2]; // char (1)
  byte satelites; // byte (1)
  bool fix;       // bool (1)
  byte fixquality;  // Same as 3D FIX
  real32_t HDOP;      // float (4) // Horizontal Dilution of Precision <2.0 is good
                    // https://en.wikipedia.org/wiki/Dilution_of_precision_(GPS)
  char datetime[20] = "dd/mm/yyyy hh:mm:ss";    // to hold GPs date time of the captured coordenates
}Payload; // Payload len =

Payload Data;


// -----------------------------------------------------------------------------
// RFM69
// -----------------------------------------------------------------------------

void radioSetup() {
    delay(10);
    radio.initialize(FREQUENCY, GATEWAYID, NETWORKID);
    radio.setHighPower();
    radio.encrypt(ENCRYPTKEY);
    radio.promiscuous(PROMISCUOUS);

}
void blinkPin( uint8_t pin, uint16_t milSec){
  pinMode( pin,OUTPUT);
  digitalWrite( pin, LOW);
  delay(milSec);
  digitalWrite(pin,HIGH);

}

void radioLoop() {
  static unsigned long oldtimer;
  unsigned numBytes;

  if (radio.receiveDone())//If some packet was received by the radio, wait for all its contents to come trough
  {
    oldtimer = millis(); //For data link loss timeout calculation
    Serial.printf("Packet received \t #Bytes:%d", radio.PAYLOADLEN);
    if (radio.TARGETID == GATEWAYID)//Check if the packet destination is the GATEWAY radio (NODE 1)
    {
      if (radio.ACKRequested()){
        Serial.printf("ACK Requested \t Data len:%d", radio.DATALEN);
        //radio.sendACK()
      }
      //Retrieve the data from the received Payload
      Data = *(Payload*)radio.DATA; //assume radio.DATA actually contains our struct and not something else

  	  if (Data.fix == 0)       //If there is No 3DFIX
  		{
        Serial.println("Quality OK!");
        Serial.print("Altitude:"); Serial.println(Data.altitude);
      }
      else
        Serial.println("Quality KO! ********");
    }
    blinkPin(LED_PIN,1000);
  }
  else  // If no packets received
  {

    if (millis() > (oldtimer + 5000))  //if no packet received for the last 5 sec consider Link Lost
      {
		  Serial.println("missing packets!");
      oldtimer=millis();
      }

  }
}
