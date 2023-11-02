#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include "TinyRF_RX.h"

// Wifi network station credentials
#define WIFI_SSID "Familia Segura"
#define WIFI_PASSWORD "Segura2526"
// Telegram BOT Token (Get from Botfather)
#define BOT_TOKEN "6009146513:AAENttCYGHG5r07itGGupmEq51ptAGay1QA" //prueba_jgsfBOT

const unsigned long BOT_MTBS = 1000; // mean time between scan messages

unsigned long bot_lasttime; // last time messages' scan has been done
X509List cert(TELEGRAM_CERTIFICATE_ROOT);
WiFiClientSecure secured_client;
UniversalTelegramBot bot(BOT_TOKEN, secured_client);

// the pin which will be connected to receiver module
// this pin has to support EXTERNAL interrupts
uint8_t rxPin = D2;

String query;
int Temperatura;
int Humedad;
int rfID;

String dataMeasuring(){
    const uint8_t bufSize = 6;
    byte buf[bufSize];
    uint8_t numRcvdBytes = 0;

    uint8_t err = getReceivedData(buf, bufSize, numRcvdBytes);


    if(err == TRF_ERR_NO_DATA){
      // query = "No data received.";
    }

    if(err == TRF_ERR_BUFFER_OVERFLOW){
      // query = "Buffer too small for received data!";
    }

    if(err == TRF_ERR_CORRUPTED){
      // query = "Received corrupted data.";
    }

    if(err == TRF_ERR_SUCCESS){
      //now we do the opposite of what we did in the transmitter sketch
      //pay attention to the '|=' operator
      //only the first byte assignment uses the '=' operator
      rfID = (int) buf[0];
      Temperatura = (int) buf[1];
      Temperatura |= (int) buf[2] << 8;
      Humedad = (int) buf[3];
      Humedad |= (int) buf[4] << 8;

      
      String strT = String(Temperatura);
      String strH = String(Humedad);

      query = "En este momento ("+ String(millis()) + ") se tiene temperatura de " + strT + "°C con humedad de " + strH + "%";
      Serial.print(query);
    }
  return query;
}

void handleNewMessages(int numNewMessages, String query, int Temperatura, int Humedad){
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);

  String answer;
  for (int i = 0; i < numNewMessages; i++) {
    telegramMessage &msg = bot.messages[i];
    Serial.println("Received " + msg.text);
    if (msg.text == "/ayuda") {
      answer = ("Los comandos en este bot son los siguientes:" "\n"
      "/ayuda : Muestra los comandos actuales." "\n"
      "/consulta : Indica cuáles son los parámetros en el momento.");
    }
    else if (msg.text == "/consulta") {
      answer = query;
    }
    else 
      answer = "Comando no existe. Favor consultar /ayuda para conocer los comandos existentes.";
    bot.sendMessage(msg.chat_id, answer, "Markdown");
  }
}

void bot_setup(){
  const String commands = F("["
                            "{\"command\":\"ayuda\",  \"description\":\"Muestra los comandos actuales.\"},"
                            "{\"command\":\"consulta\",\"description\":\"Se consultan los parámetros actuales.\"}" // no comma on last command
                            "]");
  bot.setMyCommands(commands);
  //bot.sendMessage("25235518", "Hola amigo!", "Markdown");
}

void setup(){
  Serial.begin(115200);
  Serial.println();

  //make sure you call this in your setup
  setupReceiver(rxPin);

  pinMode(LED_BUILTIN, OUTPUT);
  // attempt to connect to Wifi network:
  configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
  secured_client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  Serial.print("Connecting to Wifi SSID ");
  Serial.print(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  Serial.print("\nWiFi connected. IP address: ");
  Serial.println(WiFi.localIP());

  // Check NTP/Time, usually it is instantaneous and you can delete the code below.
  Serial.print("Retrieving time: ");
  time_t now = time(nullptr);
  while (now < 24 * 3600)
  {
    Serial.print(".");
    delay(100);
    now = time(nullptr);
  }
  Serial.println(now);

  bot_setup();
}

void loop(){
  delay(10000);
  String ans;
  ans = dataMeasuring();
  if (Temperatura != 0 && Humedad != 0){
    Serial.println(query);
    Serial.println(Temperatura);
    Serial.println(Humedad);
    Serial.println(rfID);
  }
  String alert;
  telegramMessage &msg = bot.messages[0];

  if (Temperatura > 18 || Humedad > 60) {
    alert = ("¡¡¡ALERTA!!!" "\n" + query + " en nodo" + String(rfID) + "\n" "Se detecta infringimiento de rango. Favor monitorear.");
    bot.sendMessage(msg.chat_id, alert, "Markdown");
  }

  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages, query, Temperatura, Humedad);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}