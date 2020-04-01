#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <WakeOnLan.h>
#include <UniversalTelegramBot.h>                                   // please check, if you installed ArduinoJson 5.13
#include <ESP32Ping.h>                                              // https://github.com/marian-craciunescu/ESP32Ping

const char* wifissid     = "SSID";                                  
const char* wifipassword = "xxxxxxxxxxxxx";
IPAddress ip = { 123, 456, 789, 000 };                              // target IP-Adress
const char *MACAddress = "00:00:00:00:00:00";                       // target MAC-Adress
const char* BotToken = "ooooooooo";                                 // token from the Bot-Father
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
        
        message = "Your computer is ";
        for (int i=0; i<3; i++)
        {
          millisdelay(60000);
          if(pingPC())
          {
            message += "online.";
            break;
          }
          if (i==2)
            message += "offline.";
        }
        bot.sendMessage(chat_id, message);
      }

      if (text == "/status")
      {
        String message = "STATUS\n\n";
        message += "IP: " + String(ip) + "\n";
        message += "MAC: " + String(MACAddress) + "\n\n"; 
        
        message += "your computer is ";
        if(pingPC())
          message += "online";
        else
          message += "offline";
        bot.sendMessage(chat_id, message);
      }

      if (text == "/help")
      {
        String message = "All commands for the Bot:\n";
        message += "/start welcome message\n";
        message += "/wake wake your computer\n";
        message += "/status check, if computer is online\n";
        message += "/help show this help\n";
        bot.sendMessage(chat_id, message);
      }
    }
  }
}

void wakePC() {
  WOL.setRepeat(3, 100);                                            // Optional  => repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.
  
  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // Optional  => To calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).
  WOL.sendMagicPacket(MACAddress);
}

bool pingPC() {
  int result = Ping.ping(ip);
  if( Ping.averageTime() > 0 )
    return true;
  return false;
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

void millisdelay(long ms)
{
  unsigned long stop_at = millis() + ms;
  while(millis() < stop_at) { }
}
