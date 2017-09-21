//--------------------------------------------------------------
//  define compile option here
//  if using PlatformIO compile directive -D comment out definition here
//--------------------------------------------------------------


//#define DEBUG_PORT Serial   // used on platformio for debug purpose

//  Board or CPU used - leave only one uncommented
//#define BOARD_MOTEINO
#define BOARD_ESP8266
//#define BOARD_ESP32


// Radio
#define RADIO_HIGHPOWER      1  // 1 - 0 (True - False)
#define RADIO_FREQUENCY      RF69_433MHZ //RF69_868MHZ ; RF69_915MHZ
#define RADIO_NODEID         20  //0-254 255 is broadcast
#define RADIO_GATEWAYID      1  // 0-254
#define RADIO_NETWORKID      200 //1-256
#define RADIO_PROMISCUOUS    1   // 0 - 1 (False-True)
#define RADIO_ENCRYPTKEY     "sampleEncryptKey" // 16 char key
#define RADIO_RETRYTIME      100 // ms to wait before retry
#define RADIO_NUMRETRIES     2  // number of retries before give up

#define SPI_CS                  15 //SS
#define IRQ_PIN                 4 //5
#define IS_RFM69HW              1 //0


// Json
#define MAX_JSON          200 // mas Json message

//------------------------------------------------------------------------------
// Board
//------------------------------------------------------------------------------

#define SERIAL_BAUDRATE         115200
#define LED_PIN                 2
