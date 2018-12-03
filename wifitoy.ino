#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h> // Hardware-specific library
#include <ESP8266WiFi.h>
#include <menu.h>
#include <menuIO/adafruitGfxOut.h>
#include <menuIO/chainStream.h>
#include <menuIO/serialOut.h>
#include "Adafruit_miniTFTWing.h"
#include "Beacon.h"
#include "ProgmemConsts.h"

// Basic I/O
Adafruit_miniTFTWing ss;
#define TFT_RST    -1    // we use the seesaw for resetting to save a pin
#define TFT_CS   2
#define TFT_DC   16
Adafruit_ST7735 gfx = Adafruit_ST7735(TFT_CS,  TFT_DC, TFT_RST);

#define MAX_DEPTH 2
#define textScale 2

#define ST7735_GRAY RGB565(128,128,128)

const colorDef<uint16_t> colors[] MEMMODE={
  {{ST7735_BLACK,ST7735_BLACK},{ST7735_BLACK,ST7735_BLUE,ST7735_BLUE}},//bgColor
  {{ST7735_GRAY,ST7735_GRAY},{ST7735_WHITE,ST7735_WHITE,ST7735_WHITE}},//fgColor
  {{ST7735_WHITE,ST7735_BLACK},{ST7735_YELLOW,ST7735_YELLOW,ST7735_RED}},//valColor
  {{ST7735_WHITE,ST7735_BLACK},{ST7735_WHITE,ST7735_YELLOW,ST7735_YELLOW}},//unitColor
  {{ST7735_WHITE,ST7735_GRAY},{ST7735_BLACK,ST7735_BLUE,ST7735_WHITE}},//cursorColor
  {{ST7735_WHITE,ST7735_YELLOW},{ST7735_BLUE,ST7735_RED,ST7735_RED}},//titleColor
};

Beacon *runningBeacon;
std::function<void()> screenFn;

result allstarBeacon(eventMask e, prompt &item);
result aeiouBeacon(eventMask e, prompt &item);

MENU(beaconMenu, "BeaconSpam", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,OP("All Star", allstarBeacon, Menu::enterEvent)
  ,OP("Moonbase Alpha", aeiouBeacon, Menu::enterEvent)
);

int brightness=50;
MENU(mainMenu, "WiFiToy", Menu::doNothing, Menu::noEvent, Menu::wrapStyle
  ,SUBMENU(beaconMenu)
  ,OP("Deauther", Menu::doNothing, Menu::noEvent)
  ,OP("PacketGraph", Menu::doNothing, Menu::noEvent)
  ,FIELD(brightness,"Backlight","%",0,100,10,1,Menu::doNothing, Menu::noEvent, Menu::noStyle)
);

chainStream<0> in(NULL);
MENU_OUTPUTS(out,MAX_DEPTH
  ,ADAGFX_OUT(gfx,colors,6*textScale,9*textScale,{0,0,14,4})
  ,NONE
);
NAVROOT(nav,mainMenu,MAX_DEPTH,in,out);

result beaconLooper(menuOut& o, idleEvent e) {
  switch(e) {
    case idleStart:
      screenFn();
      runningBeacon->setupWifi();
      break;
    case idling:
      runningBeacon->step();
      nav.idleChanged = true;
      break;
    case idleEnd:
      runningBeacon->teardownWifi();
      gfx.setTextSize(textScale);
      break;
  }
  return proceed;
}

result allstarBeacon(eventMask e, prompt &item) {
  screenFn = screenAllstar;
  runningBeacon = new Beacon(ssidsAllstar);
  nav.idleOn(beaconLooper);
  return proceed;
}

result aeiouBeacon(eventMask e, prompt &item) {
  screenFn = screenAeiou;
  runningBeacon = new Beacon(ssidsAeiou);
  nav.idleOn(beaconLooper);
  return proceed;
}

uint32_t buttons;
uint32_t prevButtons;

void setup() {
  Serial.begin(115200);
  Serial.println();
  ss.begin();
  ss.tftReset();   // reset the display
  ss.setBacklight(0xFFFF - (brightness * 50));  // turn down the backlight
  gfx.initR(INITR_MINI160x80);   // initialize a ST7735S chip, mini display
  gfx.setRotation(3);
  gfx.setTextSize(textScale);//text scaling
  gfx.setTextWrap(false);
  gfx.fillScreen(ST7735_BLACK);
  gfx.setTextColor(ST7735_RED,ST7735_BLACK);
  delay(1000);
  buttons = ss.readButtons();
  prevButtons = ss.readButtons();
}

void loop() {
  nav.poll();
  buttons = ss.readButtons();
  if (fallingEdge(TFTWING_BUTTON_UP)) {
    nav.doNav(downCmd);
  }
  if (fallingEdge(TFTWING_BUTTON_DOWN)) {
    nav.doNav(upCmd);
  }
  if (fallingEdge(TFTWING_BUTTON_LEFT)) {
    nav.doNav(leftCmd);
  }
  if (fallingEdge(TFTWING_BUTTON_RIGHT)) {
    nav.doNav(rightCmd);
  }
  if (fallingEdge(TFTWING_BUTTON_SELECT) || fallingEdge(TFTWING_BUTTON_B)) {
    nav.doNav(enterCmd);
  }
  if (fallingEdge(TFTWING_BUTTON_A)) {
    nav.doNav(escCmd);
  }
  prevButtons = buttons;
  ss.setBacklight(0xFFFF - (brightness * 400));  // turn down the backlight
  delay(100);
}

bool fallingEdge(uint32_t button) {
  bool buttonState = (! (buttons & button));
  bool prevButtonState = (! (prevButtons & button));
  return (!buttonState && prevButtonState);
}

void screenAllstar() {
  gfx.fillScreen(ST77XX_WHITE);
  int halfX = gfx.width()/2;
  gfx.fillCircle(halfX, 70, 35, ST77XX_GREEN);
  gfx.fillTriangle(halfX - 38, 30, halfX - 20, 22, halfX - 16, 56, ST77XX_GREEN);
  gfx.fillTriangle(halfX + 38, 30, halfX + 20, 22, halfX + 16, 56, ST77XX_GREEN);
  gfx.setCursor(10, 10);
  gfx.setTextColor(ST77XX_BLACK);
  gfx.setTextSize(4);
  gfx.println("SHRONK");
}

void screenAeiou() {
  int halfX = gfx.width()/2;
  int halfY = gfx.height()/2;
  gfx.fillScreen(ST77XX_BLACK);
  gfx.fillCircle(halfX, halfY, 54, ST77XX_BLUE);
  gfx.fillTriangle(136, 2, 20, 50, 30, 74, ST77XX_RED);
  gfx.setCursor(34, 24);
  gfx.setTextColor(ST77XX_WHITE);
  gfx.setTextSize(4);
  gfx.println("NASA");
}
