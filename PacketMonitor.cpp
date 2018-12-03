#include "PacketMonitor.h"

unsigned long pkts = 0;
unsigned long deauths = 0;

PacketMonitor::PacketMonitor(Adafruit_ST7735 *gfx) {
  _gfx = gfx;
  for (int i = 0; i <= maxRow; i++) {
    val[i] = 0;
  }
}

void PacketMonitor::setupMonitor() {
  _gfx->fillScreen(ST7735_BLACK);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(0);
  WiFi.disconnect();
  wifi_set_promiscuous_rx_cb(this->sniffer);
  wifi_set_channel(curChannel);
  wifi_promiscuous_enable(1);
}

void PacketMonitor::step() {
  curTime = millis();
  // stuff about changing channel
  if (curTime - prevTime >= 500) {
    prevTime = curTime;
    
    for (int i = 0; i < maxRow; i++) {
      val[i] = val[i + 1];
    }
    val[maxRow] = pkts;

    this->updateMultiplicator();
    
    // deauth alarm?

    this->drawScreen();
    deauths = 0;
    pkts = 0;
  }
}

void PacketMonitor::teardownMonitor() {
  wifi_set_promiscuous_rx_cb(NULL);
  wifi_promiscuous_enable(0);
  WiFi.mode(WIFI_OFF);
  wifi_set_opmode(NULL_MODE);
}

void PacketMonitor::drawScreen() {
  _gfx->fillScreen(ST7735_BLACK);
  for (int i = 0; i < maxRow; i++) {
    _gfx->drawLine(i, maxLine, i, maxLine - val[i]*multiplicator, ST7735_WHITE);
  }
}

void PacketMonitor::sniffer(uint8_t *buf, uint16_t len) {
  pkts++;
  if (buf[12] == 0xA0 || buf[12] == 0xC0) {
    deauths++;
  }
}

void PacketMonitor::updateMultiplicator() {
  maxVal = 1;
  for (int i = 0; i < maxRow; i++) {
    if (val[i] > maxVal) maxVal = val[i];
  }
  if (maxVal > maxLine) multiplicator = (double)maxLine / (double)maxVal;
  else multiplicator = 1;
}
