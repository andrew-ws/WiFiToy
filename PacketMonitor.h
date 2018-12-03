#ifndef PacketMonitor_h
#define PacketMonitor_h

#include <Adafruit_ST7735.h>
#include <ESP8266WiFi.h>
extern "C" {
  #include "user_interface.h"
}


class PacketMonitor {
public:
  PacketMonitor(Adafruit_ST7735 *gfx);
  void setupMonitor();
  void step();
  void teardownMonitor();
  static void sniffer(uint8_t *buf, uint16_t len);
private:
  void drawScreen();
  void updateMultiplicator();
  Adafruit_ST7735 *_gfx;
  int maxCh = 11;
  int minRow = 0;
  int maxRow = 159;
  int minLine = 0;
  int maxLine = 79;

  unsigned long prevTime = 0;
  unsigned long curTime = 0;
  int curChannel = 1;
  unsigned long maxVal = 0;
  double multiplicator = 0.0;

  unsigned int val[160];
};

#endif // ifndef PacketMonitor_h