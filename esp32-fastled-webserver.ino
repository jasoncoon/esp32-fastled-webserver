/*
   ESP32 FastLED WebServer: https://github.com/jasoncoon/esp32-fastled-webserver
   Copyright (C) 2017 Jason Coon

   Built upon the amazing FastLED work of Daniel Garcia and Mark Kriegsman:
   https://github.com/FastLED/FastLED

   ESP32 support provided by the hard work of Sam Guyer:
   https://github.com/samguyer/FastLED

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <FastLED.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FS.h>
#include <SPIFFS.h>
#include <EEPROM.h>

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001008)
#warning "Requires FastLED 3.1.8 or later; check github for latest code."
#endif

int timezoneOffset = -6; // Central Time
long gmtOffset_sec = timezoneOffset * 3600;
const int daylightOffset_sec = 3600;
const char* ntpServer  = "pool.ntp.org";
int timeInt = 8888;  // keeps track of what number to display
bool colon = false;

WebServer webServer(80);

const int led = 5;

uint8_t autoplay = 0;
uint8_t autoplayDuration = 10;
unsigned long autoPlayTimeout = 0;

uint8_t currentPatternIndex = 0; // Index number of which pattern is current

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

uint8_t power = 1;
uint8_t brightness = 8;

uint8_t speed = 30;

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
uint8_t cooling = 50;

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
uint8_t sparking = 120;

CRGB solidColor = CRGB::Blue;

uint8_t cyclePalettes = 0;
uint8_t paletteDuration = 10;
uint8_t currentPaletteIndex = 0;
unsigned long paletteTimeout = 0;

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#define DATA_PIN    18
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define NUM_LEDS    86
CRGBArray<NUM_LEDS> leds;

#define MILLI_AMPS         2000 // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)
#define FRAMES_PER_SECOND  60

#include "patterns.h"

#include "field.h"
#include "fields.h"

#include "secrets.h"
#include "wifi.h"
#include "web.h"

// wifi ssid and password should be added to a file in the sketch named secrets.h
// the secrets.h file should be added to the .gitignore file and never committed or
// pushed to public source control (GitHub).
// const char* ssid = "........";
// const char* password = "........";

void listDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  Serial.printf("Listing directory: %s\n", dirname);

  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }

  File file = root.openNextFile();
  while (file) {
    if (file.isDirectory()) {
      Serial.print("  DIR : ");
      Serial.println(file.name());
      if (levels) {
        listDir(fs, file.name(), levels - 1);
      }
    } else {
      Serial.print("  FILE: ");
      Serial.print(file.name());
      Serial.print("  SIZE: ");
      Serial.println(file.size());
    }
    file = root.openNextFile();
  }
}

void printLocalTime()
{
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
  timeInt = (timeinfo.tm_hour*100+timeinfo.tm_min);
}

void setup() {
  pinMode(led, OUTPUT);
  digitalWrite(led, 1);

  //  delay(3000); // 3 second delay for recovery
  Serial.begin(115200);

  SPIFFS.begin();
  listDir(SPIFFS, "/", 1);

  loadFieldsFromEEPROM(fields, fieldCount);

  setupWifi();
  setupWeb();

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setCorrection(TypicalSMD5050);

  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  
  // set master brightness control
  FastLED.setBrightness(brightness);

  autoPlayTimeout = millis() + (autoplayDuration * 1000);
}

void maskTime(int timeInt){
  // Based on the current timeInt set number segments on or off
  uint8_t c1 = 0;  // Variable to store 1s digit
  uint8_t c10 = 0;  // Variable to store 10s digit
  uint8_t c100 = 0;  // Variable to store 100s digit
  uint8_t c1000 = 0;  // Variable to store 100s digit
  int c;

  c1 = timeInt % 10;
  c10 = (timeInt / 10) % 10;
  c100 = (timeInt / 100) % 10;
  c1000 = (timeInt / 1000) % 10;
  
  CRGB color = CRGB::Black; //unused segment color
  
  //next block of if statements sets segments to black to form digits
  if (c1000 == 0) { seg1G = color; }
  if (c1000 == 1) { seg1A = seg1D = seg1E = seg1F = seg1G = color; } 
  if (c1000 == 2) { seg1C = seg1F = color; } 
  if (c1000 == 3) { seg1E = seg1F = color; } 
  if (c1000 == 4) { seg1A = seg1D = seg1E = color; } 
  if (c1000 == 5) { seg1B = seg1E = color; } 
  if (c1000 == 6) { seg1B = color; } //B
  if (c1000 == 7) { seg1D = seg1E = seg1F = seg1G = color; } 
  if (c1000 == 8) {  }
  if (c1000 == 9) { seg1D = seg1E = color; } 

  if (c100 == 0) { seg2G = color; }
  if (c100 == 1) { seg2A = seg2D = seg2E = seg2F = seg2G = color; } 
  if (c100 == 2) { seg2C = seg2F = color; } 
  if (c100 == 3) { seg2E = seg2F = color; } 
  if (c100 == 4) { seg2A = seg2D = seg2E = color; } 
  if (c100 == 5) { seg2B = seg2E = color; } 
  if (c100 == 6) { seg2B = color; } //B
  if (c100 == 7) { seg2D = seg2E = seg2F = seg2G = color; } 
  if (c100 == 8) {  }
  if (c100 == 9) { seg2D = seg2E = color; } 

  if (c10 == 0) { seg3G = color; }
  if (c10 == 1) { seg3A = seg3D = seg3E = seg3F = seg3G = color; } 
  if (c10 == 2) { seg3C = seg3F = color; } 
  if (c10 == 3) { seg3E = seg3F = color; } 
  if (c10 == 4) { seg3A = seg3D = seg3E = color; } 
  if (c10 == 5) { seg3B = seg3E = color; } 
  if (c10 == 6) { seg3B = color; } //B
  if (c10 == 7) { seg3D = seg3E = seg3F = seg3G = color; } 
  if (c10 == 8) {  }
  if (c10 == 9) { seg3D = seg3E = color; } 

  if (c1 == 0) { seg4G = color; }
  if (c1 == 1) { seg4A = seg4D = seg4E = seg4F = seg4G = color; } 
  if (c1 == 2) { seg4C = seg4F = color; } 
  if (c1 == 3) { seg4E = seg4F = color; } 
  if (c1 == 4) { seg4A = seg4D = seg4E = color; } 
  if (c1 == 5) { seg4B = seg4E = color; } 
  if (c1 == 6) { seg4B = color; } //B
  if (c1 == 7) { seg4D = seg4E = seg4F = seg4G = color; } 
  if (c1 == 8) {  }
  if (c1 == 9) { seg4D = seg4E = color; } 

  if (!colon){
    col = CRGB::Black; //turns off colon to make it blink
  }
}

void loop()
{
  handleWeb();

  if (power == 0) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
  }
  else {
    // Call the current pattern function once, updating the 'leds' array
    patterns[currentPatternIndex].pattern();
    maskTime(timeInt);

    EVERY_N_MILLISECONDS(40) {
      // slowly blend the current palette to the next
      nblendPaletteTowardPalette(currentPalette, targetPalette, 8);
      gHue++;  // slowly cycle the "base color" through the rainbow
    }

    if (autoplay == 1 && (millis() > autoPlayTimeout)) {
      nextPattern();
      autoPlayTimeout = millis() + (autoplayDuration * 1000);
    }

    if (cyclePalettes == 1 && (millis() > paletteTimeout)) {
      nextPalette();
      paletteTimeout = millis() + (paletteDuration * 1000);
    }
  }

  // send the 'leds' array out to the actual LED strip
  FastLED.show();

  // insert a delay to keep the framerate modest
  // FastLED.delay(1000 / FRAMES_PER_SECOND);
  delay(1000 / FRAMES_PER_SECOND);

  // time stuff
  EVERY_N_SECONDS(1){
    colon = !colon; // flash the colon
  }
  
  EVERY_N_SECONDS(10){
    printLocalTime();
  } 
  
  EVERY_N_MINUTES(5){
    if ( WiFi.status() == WL_CONNECTED) {
      configTime(gmtOffset_sec, daylightOffset_sec, ntpServer); //get fresh NTP time
    }
  }
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  currentPatternIndex = (currentPatternIndex + 1) % patternCount;
}

void nextPalette()
{
  currentPaletteIndex = (currentPaletteIndex + 1) % paletteCount;
  targetPalette = palettes[currentPaletteIndex];
}
