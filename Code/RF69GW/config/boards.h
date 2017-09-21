//*************************************************
//        Board specific defs
//*************************************************


//    Moteino & Moteino Mega

/*
D2 (int0), D10(SS) D11-13 (SPI) used by RF module
D8(SS) D11-13 (SPI) used by Flash (4Mbit = 512KByte)
*/


#ifdef BOARD_MOTEINO

#define LED 9

#define TX D1
#define RX D0

#define int1  D3

#define PWM1  D3
#define PWM2  D5
#define PWM3  D6

#define ANA1  D14
#define ANA2  D15
#define ANA3  D16
#define ANA4  D17
#define ANA5  D18
#define ANA6  D19
#define ANA7  A6
#define ANA8  A7

#define SPI_MOSI  D11
#define SPI_MISO  D12
#define SPI_SCK   D13

#define I2C_SCL   D19
#define I2C_SDA   D18

#endif

// ESP8266

/*
D2 (int0), D10(SS) D11-13 (SPI) used by RF module
D8(SS) D11-13 (SPI) used by Flash (4Mbit = 512KByte)
*/


#ifdef BOARD_ESP8266

#define LED 2

/* this setting not update yet */
#define TX D1
#define RX D0

#define int1  D3

#define PWM1  D3
#define PWM2  D5
#define PWM3  D6

#define ANA1  D14
#define ANA2  D15
#define ANA3  D16
#define ANA4  D17
#define ANA5  D18
#define ANA6  D19
#define ANA7  A6
#define ANA8  A7

#define SPI_MOSI  D11
#define SPI_MISO  D12
#define SPI_SCK   D13

#define I2C_SCL   D19
#define I2C_SDA   D18

#endif  // ESP8266
