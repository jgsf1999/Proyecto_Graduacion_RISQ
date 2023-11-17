#include "TinyRF_85_TX.h"

//Entre ATtiny
//  Vin    --0.1uF-- GND

//ATtiny85       ---------------   433MHz
//  5V           ---------------     Vcc
//  GND          ---------------     GND
//  P0           ---------------     DATA

void setup(){
  // transmitter default pin is pin #2. You can change it by editing Settings.h
  setupTransmitter();
}

void loop(){

  byte byteNum = 255;
  int intNum = 32767;
  long longNum = 2147483647;

  //we need a 7 bytes buffer for sending these 3 numbers
  //a byte + an int(2 bytes) + a long(4 bytes)
  //if we were to send these as text we would need 18 bytes!
  const uint8_t len = 7;
  byte msg[len];

  msg[0] = byteNum;

  //we break down the 2-byte integer into 2 single bytes like this, and store it in our byte array
  msg[1] = intNum;
  msg[2] = intNum >> 8;

  //same with the 4-byte long
  msg[3] = longNum;
  msg[4] = longNum >> 8;
  msg[5] = longNum >> 16;
  msg[6] = longNum >> 24;

  sendMulti((byte*)msg, len, 3);

  //make sure there's at least a TX_DELAY_MICROS delay between transmissions
  delayMicroseconds(TX_DELAY_MICROS);
  
}