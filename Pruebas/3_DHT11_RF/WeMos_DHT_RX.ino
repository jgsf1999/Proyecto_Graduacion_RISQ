#include "TinyRF_RX.h"

// the pin which will be connected to receiver module
// this pin has to support EXTERNAL interrupts
uint8_t rxPin = D2;

void setup(){
  Serial.begin(115200);
  //make sure you call this in your setup
  setupReceiver(rxPin);
}

void loop(){

  const uint8_t bufSize = 6;
  byte buf[bufSize];
  uint8_t numRcvdBytes = 0;

  uint8_t err = getReceivedData(buf, bufSize, numRcvdBytes);

  if(err == TRF_ERR_NO_DATA){
    return;
  }

  if(err == TRF_ERR_BUFFER_OVERFLOW){
    Serial.println("Buffer too small for received data!");
    return;
  }

  if(err == TRF_ERR_CORRUPTED){
    Serial.println("Received corrupted data.");
    return;
  }

  if(err == TRF_ERR_SUCCESS){
    int rfID;
    int Temperatura;
    int Humedad;
    //now we do the opposite of what we did in the transmitter sketch
    //pay attention to the '|=' operator
    //only the first byte assignment uses the '=' operator
    rfID = (int) buf[0];
    Temperatura = (int) buf[1];
    Temperatura |= (int) buf[2] << 8;
    Humedad = (int) buf[3];
    Humedad |= (int) buf[4] << 8;

    Serial.print(millis());
    Serial.print(",");
    Serial.print("ID:");
    Serial.print(",");
    Serial.print(rfID);
    Serial.print(",");
    Serial.print("Temperatura:");
    Serial.print(",");
    Serial.print(Temperatura);
    Serial.print(",");
    Serial.print("Humedad:");
    Serial.print(",");
    Serial.println(Humedad);
  }
  
}