
void blinkEvery( uint32_t interval, uint16_t milSec = 500){
  static uint32_t timer = 0;

  if(timer > millis()) timer= millis();
  if (timer == 0 || millis()-timer > interval) {
    blinkPin(LED, milSec);
    timer= millis();
  }
}

void blinkPin( uint8_t pin,  uint16_t milSec){
  pinMode( pin,OUTPUT);
  digitalWrite( pin, LOW);
  delay(milSec);
  digitalWrite(pin,HIGH);
}
