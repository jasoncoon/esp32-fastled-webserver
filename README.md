# ESP32 FastLED Web Server
**Work in progress ESP32 port of https://github.com/jasoncoon/esp8266-fastled-webserver**

Control addressable LEDs with an ESP32 via a web browser over Wi-Fi.

## Features
### Currently Working:
* [x] DemoReel100 patterns
* [x] [Sam's multi-core support](https://github.com/samguyer/FastLED/blob/master/examples/DemoReelESP32/DemoReelESP32.ino)
* [x] Static web app file serving from SPIFFS
* [x] Ability to adjust these settings via the HTTP REST API:
   * [x] power on/off
   * [x] brightness
   * [x] pattern
   * [x] autoplay on/off
   * [x] autoplay duration
   * [x] speed
   * [x] palette
   * [x] auto palette cycling
   * [x] palette duration
   * [x] solid color
   * [x] twinkle speed/density
   * [x] fire cooling/sparking
* [x] Setting storage in EEPROM

### Currently Lacking:
* [ ] WebSockets for automatically refreshing/syncing web clients

## Requirements

### Hardware

#### ESP32 Development Board

[![Adafruit HUZZAH32 – ESP32 Feather Board](https://cdn-learn.adafruit.com/assets/assets/000/041/619/thumb100/feather_3405_iso_ORIG.jpg?1494445509)](https://www.adafruit.com/product/3405)

[Adafruit HUZZAH32 – ESP32 Feather Board](https://www.adafruit.com/product/3405)

**or**

[WEMOS LOLIN D32](https://wiki.wemos.cc/products:d32:d32)

**or**

[An ESP32 development board of your choice](https://www.google.com/search?q=esp32+development+board)

#### Addressable LED strip

[![Adafruit NeoPixel Ring](https://www.adafruit.com/images/145x109/1586-00.jpg)](https://www.adafruit.com/product/1586)

[Adafruit NeoPixel Ring](https://www.adafruit.com/product/1586)

#### Other hardware:

* [3.3V to 5V Logic Level Shifter](http://www.digikey.com/product-detail/en/texas-instruments/SN74HCT245N/296-1612-5-ND/277258) (required if LEDs "glitch")
* [Octo Level Shifter FeatherWing](https://www.evilgeniuslabs.org/level-shifter-featherwing) (tidy level shifter PCB)

Recommended by [Adafruit NeoPixel "Best Practices"](https://learn.adafruit.com/adafruit-neopixel-uberguide/best-practices) to help protect LEDs from current onrush:
* [1000µF Capacitor](http://www.digikey.com/product-detail/en/panasonic-electronic-components/ECA-1EM102/P5156-ND/245015)
* [300 to 500 Ohm resistor](https://www.digikey.com/product-detail/en/stackpole-electronics-inc/CF14JT470R/CF14JT470RCT-ND/1830342)

### Software

* [Arduino](https://www.arduino.cc/en/main/software)
* [ESP32 Arduino Libraries & Tools](https://github.com/espressif/arduino-esp32)
* [Arduino ESP32 filesystem uploader](https://github.com/me-no-dev/arduino-esp32fs-plugin)

#### Libraries

* [samguyer/FastLED](https://github.com/samguyer/FastLED)
* [ESP32 Arduino Libraries & Tools](https://github.com/espressif/arduino-esp32)
* [ESP8266/ESP32 WebServer](https://github.com/bbx10/WebServer_tng)

#### SPIFFS fix

To fix an [issue with serving files from SPIFFS over the web server](https://github.com/jasoncoon/esp32-fastled-webserver/issues/1), I used Yves BAZIN's fix here: https://github.com/hpwit/lib

Copy [libspiffs.a](https://github.com/jasoncoon/esp32-fastled-webserver/raw/master/libspiffs.a) to the ESP32 tools directory:
`\Documents\Arduino\hardware\espressif\esp32\tools\sdk\lib\libspiffs.a`

And copy [esp32fs.jar](https://github.com/jasoncoon/esp32-fastled-webserver/raw/master/esp32fs.jar) to the Arduino tools directory:
`\Documents\Arduino\tools\ESP32FS\tool\esp32fs.jar`
