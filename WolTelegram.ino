#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <WakeOnLan.h>
#include <UniversalTelegramBot.h>                                   // please check, if you installed ArduinoJson 5.13

const char* wifissid     = "SSID";                                  
const char* wifipassword = "xxxxxxxxxxxxx";
const char *MACAddress = "00:00:00:00:00:00";                       // target
char* BotToken = "ooooooooo";                                       // token from the Bot-Father
String chatIDs[] = { "9876543210" };                                // insert the allowed ShatIDs

int Bot_mtbs = 1000; //mean time between scan messages
long Bot_lasttime;   //last time messages' scan has been done

WiFiUDP UDP;
WakeOnLan WOL(UDP);
WiFiClientSecure client;
UniversalTelegramBot bot(BotToken, client);

void setup() {
  pinMode(2, OUTPUT);

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifissid, wifipassword);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  blink(500); //wifi is connected
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    
    Bot_lasttime = millis();
  }
}

void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++)
  {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;
    String from_name = bot.messages[i].from_name;
    if (from_name == "") from_name = "Guest";

    if( stringArrayContains(chatIDs, chat_id) )
    {
      if (text == "/start")
      {
        String message = "Welcome " + from_name + " to the ultimate WolTelegram-Bot.\n";
        message += "You can start your Computer with /wake\n";
        bot.sendMessage(chat_id, message);
      }
  
      if (text == "/wake")
      {
        String message = "I will send the Wake-On-Lan-Package.\n";
        message += "... ... ...";
        bot.sendMessage(chat_id, message);
        wakePC();
      }
    }
  }
}

void wakePC() {
  WOL.setRepeat(3, 100);                                            // Optional  => repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.
  
  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // Optional  => To calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).
  WOL.sendMagicPacket(MACAddress);
}

void blink(int ms) {
  digitalWrite(2, HIGH);
  delay(ms);
  digitalWrite(2, LOW);
  delay(ms);
  digitalWrite(2, HIGH);
  delay(ms);
  digitalWrite(2, LOW);
}

boolean stringArrayContains(String arr[], String w) {
  for (int i=0; i<sizeof(arr); i++)
  {
    if(arr[i] == w) {
      return true;
    }
  }
  return false;
}
