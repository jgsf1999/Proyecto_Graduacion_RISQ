#include "TinyRF_85_TX.h"
//Entre ATtiny
//  Vin    --0.1uF-- GND

//ATtiny85       ---------------   433MHz
//  5V           ---------------     Vcc
//  GND          ---------------     GND
//  P0           ---------------     DATA
void setup(){

  setupTransmitter();
}

void loop(){

  const char* msg1 = "hi";
  const char* msg2 = "bye";

  sendMulti((byte*)msg1, strlen(msg1), 10);
  delay(5000);
  sendMulti((byte*)msg2, strlen(msg2), 10);

  //make sure there's at least a TX_DELAY_MICROS delay between transmissions
  //otherwise the receiver's behavior will be undefined
  delay(10000);
  
}