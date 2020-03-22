#include <WiFi.h>
#include <WiFiUdp.h>

#include <WakeOnLan.h>

WiFiUDP UDP;
WakeOnLan WOL(UDP);

const char* wifissid     = "SSID";
const char* wifipassword = "xxxxxxxxxxxxx";

void wakePC() {
  const char *MACAddress = "00:1B:78:B5:56:17";
  WOL.setRepeat(3, 100);                                            // Optional  => repeat the packet three times with 100ms between. WARNING delay() is used between send packet function.
  
  WOL.calculateBroadcastAddress(WiFi.localIP(), WiFi.subnetMask()); // Optional  => To calculate the broadcast address, otherwise 255.255.255.255 is used (which is denied in some networks).
  WOL.sendMagicPacket(MACAddress);
}

void setup() {

  WiFi.mode(WIFI_STA);
  WiFi.begin(wifissid, wifipassword);

  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }

  wakePC();
}


void loop() {
  
}
