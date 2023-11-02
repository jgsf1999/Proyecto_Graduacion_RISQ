//Libraries needed TinyDHT for temperature and humidity measure with
//DHT11 in ATtiny85, TinyRF_85_TX for trasnmission with ATtiny85

#include "TinyDHT.h"
#include <TinyRF_85_TX.h>
#include <tinysnore.h>

//Circuit setup
//Only ATtiny
//  Vin    --0.1uF-- GND

//ATtiny with DHT11 and 433MHz module
//433MHz ----- ATtiny85 ----- DHT11
//  VCC  -----   5V    -----   VCC/+
//  GND  -----   GND   -----   GND/-
//               P2    -----  DATA/out
//  DATA  -----  P0

/**
 * TinyRF sends its data as an array of bytes
 * Which means any data can be send not just text characters
 * I've seen people send numbers as text characters so I included this 
 * example to show how numbers can be sent as byte array
 * This saves RAM and makes transmissions shorter
**/

#define led 1     // LED pin for ATtiny
#define DHTPIN 2  // DHT connected to ATtiny pin P2

#define DHTTYPE DHT11   // DHT 11 

DHT dht(DHTPIN, DHTTYPE);

void setup(){
  //setup for LED (indicator)
  pinMode(led, OUTPUT);

  //setup for DHT
  dht.begin();

  //setup for RF communication (transmitter)
  setupTransmitter();
}

void loop(){

  int ID = 1;                         //node number
  delay(2000);                        //delay needed for DHT11 to estabilize
  int t = dht.readTemperature();
  int h = dht.readHumidity();
  if (t > 18 || h > 60) {             //conditions for the LED to turn on (failsafe)
    digitalWrite(led, HIGH);
    delay(500); // Wait for 1000 millisecond(s)
    digitalWrite(led, LOW);
  }
  //we need a 7 bytes buffer for sending these 3 numbers
  //a byte + an int(2 bytes) + a long(4 bytes)
  //if we were to send these as text we would need 18 bytes!
  const uint8_t len = 6;
  byte msg[len];


  //we break down the 2-byte integer into 2 single bytes like this, and store it in our byte array
  msg[0] = ID;
  msg[1] = t;
  msg[2] = t >> 8;
  msg[3] = h;
  msg[4] = h >> 8;

  sendMulti((byte*)msg, len, 3);

  //make sure there's at least a TX_DELAY_MICROS delay between transmissions
  delay(1500);
  uint32_t snore_time = 95000 + (ID - 1)*10000;
  snore(snore_time);
  
}