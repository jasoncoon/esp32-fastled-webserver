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

#include "palettes.h";
#include "twinkleFOX.h"

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, speed);
}

void addGlitter( fract8 chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(speed, 0, NUM_LEDS - 1);
  static int prevpos = 0;
  CRGB color = ColorFromPalette(palettes[currentPaletteIndex], gHue, 255);
  if ( pos < prevpos ) {
    fill_solid( leds + pos, (prevpos - pos) + 1, color);
  } else {
    fill_solid( leds + prevpos, (pos - prevpos) + 1, color);
  }
  prevpos = pos;
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t beat = beatsin8( speed, 64, 255);
  CRGBPalette16 palette = palettes[currentPaletteIndex];
  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16( i + speed, 0, NUM_LEDS - 1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void showSolidColor()
{
  fill_solid(leds, NUM_LEDS, solidColor);
}

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

PatternAndNameList patterns = {
  // TwinkleFOX patterns
  { drawTwinkles, "Twinkles" },
  //  { retroC9Twinkles,        "Retro C9 Twinkles" },
  //  { redWhiteTwinkles,       "Red & White Twinkles" },
  //  { blueWhiteTwinkles,      "Blue & White Twinkles" },
  //  { redGreenWhiteTwinkles,  "Red, Green & White Twinkles" },
  //  { fairyLightTwinkles,     "Fairy Light Twinkles" },
  //  { snow2Twinkles,          "Snow 2 Twinkles" },
  //  { hollyTwinkles,          "Holly Twinkles" },
  //  { iceTwinkles,            "Ice Twinkles" },
  //  { partyTwinkles,          "Party Twinkles" },
  //  { forestTwinkles,         "Forest Twinkles" },
  //  { lavaTwinkles,           "Lava Twinkles" },
  //  { fireTwinkles,           "Fire Twinkles" },
  //  { cloud2Twinkles,         "Cloud 2 Twinkles" },
  //  { oceanTwinkles,          "Ocean Twinkles" },

  // DemoReel100 patterns
  { rainbow, "rainbow" },
  { rainbowWithGlitter, "rainbowWithGlitter" },
  { confetti, "confetti" },
  { sinelon, "sinelon" },
  { juggle, "juggle" },
  { bpm, "bpm" },

  { showSolidColor,         "Solid Color" },
};

const uint8_t patternCount = ARRAY_SIZE(patterns);

