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

const uint8_t matrixWidth = 255;
const uint8_t matrixHeight = 255;

#define MAX_DIMENSION ((matrixWidth > matrixHeight) ? matrixWidth : matrixHeight)

// The 16 bit version of our coordinates
uint16_t noisex;
uint16_t noisey;
uint16_t noisez;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
int noisespeedx = 0;
int noisespeedy = 1;
int noisespeedz = 0;

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise will be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.
uint16_t noisescale = 1; // scale is set dynamically once we've started up

// This is the array that we keep our computed noise values in
uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];

uint8_t colorLoop = 0;

CRGBPalette16 blackAndWhiteStripedPalette;

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( blackAndWhiteStripedPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  blackAndWhiteStripedPalette[0] = CRGB::White;
  blackAndWhiteStripedPalette[4] = CRGB::White;
  blackAndWhiteStripedPalette[8] = CRGB::White;
  blackAndWhiteStripedPalette[12] = CRGB::White;

}

CRGBPalette16 blackAndBlueStripedPalette;

// This function sets up a palette of black and blue stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndBlueStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( blackAndBlueStripedPalette, 16, CRGB::Black);

  for (uint8_t i = 0; i < 6; i++) {
    blackAndBlueStripedPalette[i] = CRGB::Blue;
  }
}

// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.

void drawNoise(CRGBPalette16 palette, uint8_t hueReduce = 0)
{
  for (uint8_t i = 0; i < NUM_LEDS; i++) {
    uint8_t x = coordsX[i];
    uint8_t y = coordsY[i];

    int xoffset = noisescale * x;
    int yoffset = noisescale * y;

    uint8_t data = inoise8(x + xoffset + noisex, y + yoffset + noisey, noisez);

    // The range of the inoise8 function is roughly 16-238.
    // These two operations expand those values out to roughly 0..255
    // You can comment them out if you want the raw noise data.
    data = qsub8(data, 16);
    data = qadd8(data, scale8(data, 39));

    if (hueReduce > 0 && data >= hueReduce)
      data -= hueReduce;

    leds[i] = ColorFromPalette(palette, data, 255, LINEARBLEND);
  }

  noisex += noisespeedx;
  noisey += noisespeedy;
  noisez += noisespeedz;
}

void rainbowNoise() {
  noisespeedx = 0;
  noisespeedy = -1;
  noisespeedz = 0;
  noisescale = 6;
  colorLoop = 0;
  drawNoise(RainbowColors_p);
}

void rainbowStripeNoise() {
  noisespeedx = 0;
  noisespeedy = -2;
  noisespeedz = 0;
  noisescale = 6;
  colorLoop = 0;
  drawNoise(RainbowStripeColors_p);
}

void partyNoise() {
  noisespeedx = -9;
  noisespeedy = 0;
  noisespeedz = 0;
  noisescale = 8;
  colorLoop = 0;
  drawNoise(PartyColors_p);
}

void forestNoise() {
  noisespeedx = -9;
  noisespeedy = 0;
  noisespeedz = 0;
  noisescale = 8;
  colorLoop = 0;
  drawNoise(ForestColors_p);
}

void cloudNoise() {
  noisespeedx = -2;
  noisespeedy = 0;
  noisespeedz = 0;
  noisescale = 6;
  colorLoop = 0;
  drawNoise(CloudColors_p);
}

void fireNoise() {
  noisespeedx = 0; // 24;
  noisespeedy = -32;
  noisespeedz = 0;
  noisescale = 16;
  colorLoop = 0;
  drawNoise(HeatColors_p, 60);
}

void fireNoise2() {
  noisespeedx = 0;
  noisespeedy = -8;
  noisespeedz = 3;
  noisescale = 8;
  colorLoop = 0;
  drawNoise(HeatColors_p);
}

void lavaNoise() {
  noisespeedx = 0;
  noisespeedy = -1;
  noisespeedz = 1;
  noisescale = 6;
  colorLoop = 0;
  drawNoise(LavaColors_p);
}

void oceanNoise() {
  noisespeedx = -2;
  noisespeedy = 0;
  noisespeedz = 4;
  noisescale = 6;
  colorLoop = 0;
  drawNoise(OceanColors_p);
}

void blackAndWhiteNoise() {
  SetupBlackAndWhiteStripedPalette();
  noisespeedx = -12;
  noisespeedy = 0;
  noisespeedz = 0;
  noisescale = 6;
  colorLoop = 0;
  drawNoise(blackAndWhiteStripedPalette);
}

void blackAndBlueNoise() {
  SetupBlackAndBlueStripedPalette();
  noisespeedx = 0;
  noisespeedy = 8;
  noisespeedz = 0;
  noisescale = 8;
  colorLoop = 0;
  drawNoise(blackAndBlueStripedPalette);
}
