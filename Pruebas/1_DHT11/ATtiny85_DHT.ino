#include "TinyDHT.h"

// ATtiny85 ----- DHT11
//    5V    -----   VCC/+
//    GND   -----   GND/-
//    P2    -----  DATA/out

#define DHTPIN 2  // DHT connected to ATtiny pint P2
#define led 1

#define DHTTYPE DHT11   // DHT 11 
//#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)


DHT dht(DHTPIN, DHTTYPE);

void setup() {
  pinMode(led, OUTPUT);
  dht.begin();
}

void loop() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  delay(2000);
  int t = dht.readTemperature(1);
  int h = dht.readHumidity();

  // check if returns are valid then something went wrong!
  if ( t > 0 && h > 0) { // if correct condition    
      digitalWrite(led, HIGH); //LED stays on if it correctly reads humidity      

  } else {
      digitalWrite(led, LOW);
  }
}
