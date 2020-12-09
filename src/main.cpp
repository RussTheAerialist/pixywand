#include <Arduino.h>
#include <M5StickC.h>
#include <FastLED.h>
#include "ble.h"
#define LED_CNT 14
#define TOP_PIN 26
#define BOTTOM_PIN 0
#define COLOR_CTRL yaw
#define BRIGHTNESS_CTRL pitch
#define ANIMATION_CTRL roll

CRGB realleds[LED_CNT];
CRGBSet leds(realleds, LED_CNT);
BLESerial ble;

uint8_t i = 0;
float pitch, roll, yaw;
uint8_t hue;
uint8_t width;
uint32_t color = WHITE;
// static TaskHandle_t LedShowTaskHandle = 0;

void LedShowTask(void *_pvParameters) {
  for(;;) {
    FastLED.show();
    EVERY_N_MILLISECONDS(100) {
      leds(i, (i+ width) % LED_CNT) = CRGB::Black;
      i = (i+1)%LED_CNT;
      leds(i, (i+ width) % LED_CNT) = CHSV(hue, 255, 255);
    }
  }
}

void setupLeds() {
  FastLED.addLeds<WS2812, TOP_PIN, GRB>(leds, LED_CNT).setCorrection(TypicalLEDStrip);
  FastLED.addLeds<WS2812, BOTTOM_PIN, GRB>(leds, LED_CNT).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(10);
  xTaskCreatePinnedToCore(LedShowTask, "FastLedshowTask", 2048, NULL, 2, NULL, 1); // Run this on the first core.
}

void setup() {
  M5.begin();

  int x = M5.IMU.Init();
  if (x) {
    color = RED;
  }
  M5.Lcd.setRotation(3);
  M5.Lcd.setTextColor(BLACK, WHITE);
  M5.Lcd.fillScreen(color);

  ble.setup();
  setupLeds();
}

void loop() {
  M5.update();
  if (M5.BtnA.pressedFor(2000)) M5.Axp.PowerOff();
  // M5.Lcd.fillScreen(color);
  M5.IMU.getAhrsData(&pitch, &roll, &yaw);
  hue = (uint8_t)map(COLOR_CTRL, -180, 180, 0, 255);
  width = (uint8_t)map(BRIGHTNESS_CTRL, -180, 180, 0, 14);

  M5.Lcd.setCursor(0,0);
  // M5.Lcd.fillRect(0, 0, 80, 80, color);
  M5.Lcd.printf(" p=%.2f\n r=%.2f\n y=%.2f\n", pitch, roll, yaw);
  ble.update();

  delay(10);
}
