#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>

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


void handleNewMessages(int numNewMessages)
{
  Serial.print("handleNewMessages ");
  Serial.println(numNewMessages);
  
  String answer;
  for (int i = 0; i < numNewMessages; i++) {
    telegramMessage &msg = bot.messages[i];
    Serial.println("Received " + msg.text);
    if (msg.text == "/ayuda") {
      answer = ("Los comandos en este bot son los siguientes:" "\n"
      "/ayuda : Muestra los comandos actuales." "\n"
      "/activar : Activación de alertas, se empieza el envío mensajes." "\n"
      "/desactivar : Desactivación de alertas, se detiene el envío de mensajes." "\n"
      "/consulta : Indica cuáles son los parámetros en el momento.");
    }
    else if (msg.text == "/consulta") {
      answer = "Se consultan los parámetros actuales.";
    }
    else 
      answer = "Comando no existe. Favor consultar /ayuda para conocer los comandos existentes.";
    bot.sendMessage(msg.chat_id, answer, "Markdown");
  }
}

void bot_setup()
{
  const String commands = F("["
                            "{\"command\":\"ayuda\",  \"description\":\"Muestra los comandos actuales.\"},"
                            "{\"command\":\"consulta\",\"description\":\"Se consultan los parámetros actuales.\"}" // no comma on last command
                            "]");
  bot.setMyCommands(commands);
  //bot.sendMessage("25235518", "Hola amigo!", "Markdown");
}

void setup()
{
  Serial.begin(115200);
  Serial.println();


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

void loop()
{
  if (millis() - bot_lasttime > BOT_MTBS)
  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while (numNewMessages)
    {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }

    bot_lasttime = millis();
  }
}
