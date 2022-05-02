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

// The larger (16-bit) version of our coordinates
static uint16_t noisex;
static uint16_t noisey;
static uint16_t noisez;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
static int noisespeedx = 0;
static int noisespeedy = 1;
static int noisespeedz = 0;

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise will be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.
static uint16_t noisescale = 1; // scale is set dynamically once we've started up

uint8_t noise[256][256];

// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.
CRGB noiseXYZ(CRGBPalette16 palette, int x, int y, int z, uint16_t i)
{
  uint8_t j = coordsX[i];
  uint8_t k = coordsY[i];

  uint8_t data = inoise8(x, y, z);
  // The range of the inoise8 function is roughly 16-238.
  // These two operations expand those values out to roughly 0..255
  // You can comment them out if you want the raw noise data.
  data = qsub8(data, 16);
  data = qadd8(data, scale8(data, 39));

  uint8_t dataSmoothing = 0;

  uint8_t speed = noisespeedx + noisespeedy + noisespeedz;
  if (speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }

  if (dataSmoothing) {
    uint8_t olddata = noise[j][k];
    uint8_t newdata = scale8(olddata, dataSmoothing) + scale8(data, 256 - dataSmoothing);
    data = newdata;
  }
  
  noise[j][k] = data;

  return ColorFromPalette(palette, data, 255, LINEARBLEND);
}

void drawNoise(CRGBPalette16 palette)
{
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t x = coordsX[i];
    uint8_t y = coordsY[i];

    int xoffset = noisescale * x;
    int yoffset = noisescale * y;

    leds[i] = noiseXYZ(palette, x + xoffset + noisex, y + yoffset + noisey, noisez, i);
  }

  noisex += noisespeedx;
  noisey += noisespeedy;
  noisez += noisespeedz;
}

void paletteNoise() {
  noisespeedx = 2;
  noisespeedy = 0;
  noisespeedz = 0;
  noisescale = 1;
  drawNoise(currentPalette);
}

void drawPolarNoise(CRGBPalette16 palette)
{
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    uint8_t x = angles[i];
    uint8_t y = radii[i] / 2u; // divide by 2 to change range of values from [0..255] to [0..127]

    int xoffset = noisescale * x;
    int yoffset = noisescale * y;
    leds[i] = noiseXYZ(palette, x + xoffset + noisex, y + yoffset + noisey, noisez, i);
  }
  noisex += noisespeedx;
  noisey += noisespeedy;
  noisez += noisespeedz;
}

void polarNoise() {
  noisespeedx = 2;
  noisespeedy = 0;
  noisespeedz = 0;
  noisescale = 1;
  drawPolarNoise(currentPalette);
}
