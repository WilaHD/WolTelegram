// Some Bug-Fix

#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiClientSecure.h>
#include <WakeOnLan.h>
#include <UniversalTelegramBot.h>                                   // please check, that you installed ArduinoJson 5.13
#inchandleNewMessageslude <ESP32Ping.h>                             // https://github.com/marian-craciunescu/ESP32Ping

const char* wifissid     = "SSID";
const char* wifipassword = "xxxxxxxxxxxxx";

IPAddress ip(123, 456, 789, 000);                                   // target IP-Adress
const char *MACAddress = "00:00:00:00:00:00";                       // target MAC-Adress
const char *BotToken = "ooooooooo";                                 // token from the Bot-Father
const String chatIDs[] = { "9876543210", "123123132" };             // whitelist for all allowed ChatIDs

const int Bot_mtbs = 1000;  //mean time between scan messages
long Bot_lasttime;          //last time messages' scan has been done

WiFiUDP UDP;
WakeOnLan WOL(UDP);
WiFiClientSecure client;
UniversalTelegramBot bot(BotToken, client);

void setup() {
  pinMode(2, OUTPUT);     // Blink-LED

  WiFi.mode(WIFI_STA);    
  WiFi.begin(wifissid, wifipassword);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  blink(500); //wifi is connected
}

void loop() {
  if (millis() > Bot_lasttime + Bot_mtbs) {                                 //check every second for messages
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    if(numNewMessages) {
      handleNewMessages(numNewMessages);
    }
    
    Bot_lasttime = millis();
  }
}

void handleNewMessages(int numNewMessages) {
  String chat_id = String(bot.messages[numNewMessages-1].chat_id);
  String text = bot.messages[numNewMessages-1].text;
  String from_name = bot.messages[numNewMessages-1].from_name;
  if (from_name == "") from_name = "Guest";

  if (!stringArrayContains(chatIDs, chat_id) ) {    // check if chatID is allowed
    return; // send no message
  }
  
  if (text handleNewMessages== "/start") {
    String message = "Welcome " + from_name + " to the ultimate WolTelegram-Bot.\n";
    message += "Type /help to show all commands.\n";
    bot.sendMessage(chat_id, message);
  }

  else if (text == "/wake") {
    String message = "I will send the Wake-On-Lan-Package.\n";
    message += "... ... ...";
    bot.sendMessage(chat_id, message);
    wakePC();
    
    message = "Your computer is ";
    for (int i=0; i<3; i++) {
      millisdelay(60000);
      if(pingPC()) {
        message += "online.";
        break;
      }
      if (i==2) {
        message += "offline.";
      }
    }
    bot.sendMessage(chat_id, message);
  }

  else if (text == "/status") {
    String message = "STATUS\n\n";
    message += "---------------------------------------\n";
    message += "IP:  " + String(ip[0]) + "." + String(ip[1]) + "."+ String(ip[2]) + "."+ String(ip[3]) + "\n";
    message += "MAC: " + String(MACAddress) + "\n"; 
    message += "---------------------------------------\n\n";
    message += "your computer is ";
    if(pingPC()) {
      message += "online";
    } else {
      message += "offline";
    }
    bot.sendMessage(chat_id, message);
  }

  else if (text == "/coffee") {
    String message = "Coffee is a brewed drink prepared from roasted coffee beans, the seeds of berries from certain Coffea species. Once ripe, coffee berries are picked, processed, and dried. Dried coffee seeds are roasted to varying degrees, depending on the desired flavor.";
    bot.sendMessage(chat_id, message);
  }
  
  else if (text == "/help") {
    String message = "All commands for the Bot:\n";
    message += "/start welcome message\n";
    message += "/wake wake your computer\n";
    message += "/status check, if computer is online\n";
    message += "/help show this help\n";
    bot.sendMessage(chat_id, message);
  }
  
  else if (text[0] == '/') {
    String message = "Unokwn command!\nType /help for help.";
    bot.sendMessage(chat_id, message);
  }
  
  
}

void wakePC() {
  WOL.setRepeat(3, 100);                                            // Optional  => repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.
  
  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // Optional  => To calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).
  WOL.sendMagicPacket(MACAddress);
}

bool pingPC() {
  return Ping.ping(ip);  //send a ping to ip
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
  for (int i=0; i<sizeof(arr); i++) {
    if(arr[i] == w) {
      return true;
    }
  }
  return false;
}

void millisdelay(long ms) {
  unsigned long stop_at = millis() + ms;
  while(millis() < stop_at) { }
}
