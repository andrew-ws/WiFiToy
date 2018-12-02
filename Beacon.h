#ifndef Beacon_h
#define Beacon_h

#include <ESP8266WiFi.h>
#include <osapi.h>
extern "C" {
  #include "user_interface.h"
  typedef void (*freedom_outside_cb_t)(uint8 status);
  int wifi_register_send_pkt_freedom_cb(freedom_outside_cb_t cb);
  void wifi_unregister_send_pkt_freedom_cb(void);
  int wifi_send_pkt_freedom(uint8 *buf, int len, bool sys_seq);
}

class Beacon {
  public:
    Beacon(const char* ssids);
    void setupWifi();
    void step();
    void teardownWifi();
  private:
    const char* _ssids;
    char emptySSID[32];
    const uint8_t channels[3] = {1, 6, 11};
    const bool wpa2 = false;
    uint8_t channelIndex = 0;
    uint8_t macAddr[6];
    uint8_t wifi_channel = 1;
    uint32_t currentTime = 0;
    uint32_t packetSize = 0;
    uint32_t attackTime = 0;
    void randomMac();
    void nextChannel();

    // beacon frame definition
    uint8_t beaconPacket[109] = {
      /*  0 - 3  */ 0x80, 0x00, 0x00, 0x00, // Type/Subtype: managment beacon frame
      /*  4 - 9  */ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Destination: broadcast
      /* 10 - 15 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source
      /* 16 - 21 */ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, // Source

      // Fixed parameters
      /* 22 - 23 */ 0x00, 0x00, // Fragment & sequence number (will be done by the SDK)
      /* 24 - 31 */ 0x83, 0x51, 0xf7, 0x8f, 0x0f, 0x00, 0x00, 0x00, // Timestamp
      /* 32 - 33 */ 0xe8, 0x03, // Interval: 0x64, 0x00 => every 100ms - 0xe8, 0x03 => every 1s
      /* 34 - 35 */ 0x31, 0x00, // capabilities Tnformation

      // Tagged parameters

      // SSID parameters
      /* 36 - 37 */ 0x00, 0x20, // Tag: Set SSID length, Tag length: 32
      /* 38 - 69 */ 0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20,
      0x20, 0x20, 0x20, 0x20, // SSID

      // Supported Rates
      /* 70 - 71 */ 0x01, 0x08, // Tag: Supported Rates, Tag length: 8
      /* 72 */ 0x82, // 1(B)
      /* 73 */ 0x84, // 2(B)
      /* 74 */ 0x8b, // 5.5(B)
      /* 75 */ 0x96, // 11(B)
      /* 76 */ 0x24, // 18
      /* 77 */ 0x30, // 24
      /* 78 */ 0x48, // 36
      /* 79 */ 0x6c, // 54

      // Current Channel
      /* 80 - 81 */ 0x03, 0x01, // Channel set, length
      /* 82 */      0x01,       // Current Channel

      // RSN information
      /*  83 -  84 */ 0x30, 0x18,
      /*  85 -  86 */ 0x01, 0x00,
      /*  87 -  90 */ 0x00, 0x0f, 0xac, 0x02,
      /*  91 -  92 */ 0x02, 0x00,
      /*  93 - 100 */ 0x00, 0x0f, 0xac, 0x04, 0x00, 0x0f, 0xac, 0x04, /*Fix: changed 0x02(TKIP) to 0x04(CCMP) is default. WPA2 with TKIP not supported by many devices*/
      /* 101 - 102 */ 0x01, 0x00,
      /* 103 - 106 */ 0x00, 0x0f, 0xac, 0x02,
      /* 107 - 108 */ 0x00, 0x00
    };

};

#endif // ifndef Beacon_h