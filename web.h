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
//String paramcheck(String PARAM_MESSAGE, AsyncWebServerRequest *request){
//  String message;
//        if (request->hasParam(PARAM_MESSAGE)) {
//            message = request->getParam(PARAM_MESSAGE)->value();
//        } else {
//            message = "No message sent";
//        }
//        return message;
//}
void setupWeb()
{
  webServer.on("/all", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(led, 0);
    String json = getFieldsJson(fields, fieldCount);
    request->send(200, "text/json", json);
    digitalWrite(led, 1);
  });

  webServer.on("/fieldValue", HTTP_GET, [](AsyncWebServerRequest *request) {
    digitalWrite(led, 0);
    String name = request->getParam("name")->value();
    String value = getFieldValue(name, fields, fieldCount);
    request->send(200, "text/json", value);
    digitalWrite(led, 1);
  });

  webServer.on("/fieldValue", HTTP_POST, [](AsyncWebServerRequest *request) {
    digitalWrite(led, 0);
    String name = request->getParam("name")->value();
    String value = request->getParam("value")->value();
    String newValue = setFieldValue(name, value, fields, fieldCount, request);
    request->send(200, "text/json", newValue);
    digitalWrite(led, 1);
  });
  //First request will return 0 results unless you start scan from somewhere else (loop/setup)
  //Do not request more often than 3-5 seconds
  webServer.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request) {
    String json = "[";
    int n = WiFi.scanComplete();
    if (n == -2)
    {
      WiFi.scanNetworks(true);
    }
    else if (n)
    {
      for (int i = 0; i < n; ++i)
      {
        if (i)
          json += ",";
        json += "{";
        json += "\"rssi\":" + String(WiFi.RSSI(i));
        json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
        json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
        json += ",\"channel\":" + String(WiFi.channel(i));
        json += ",\"secure\":" + String(WiFi.encryptionType(i));
        //      json += ",\"hidden\":"+String(WiFi.isHidden(i)?"true":"false");
        json += "}";
      }
      WiFi.scanDelete();
      if (WiFi.scanComplete() == -2)
      {
        WiFi.scanNetworks(true);
      }
    }
    json += "]";
    request->send(200, "application/json", json);
    json = String();
  });

  webServer.serveStatic("/", SPIFFS, "/", "max-age=86400").setDefaultFile("index.htm");
  webServer.serveStatic("/index.htm", SPIFFS, "/index.htm", "max-age=86400");
  webServer.serveStatic("/favicon.ico", SPIFFS, "/favicon.ico", "max-age=86400");
  webServer.serveStatic("/css/styles.css", SPIFFS, "/css/styles.css", "max-age=86400");
  webServer.serveStatic("/js/app.js", SPIFFS, "/js/app.js", "max-age=86400");
  webServer.serveStatic("/images/atom196.png", SPIFFS, "/images/atom196.png", "max-age=86400");

  webServer.begin();
  Serial.println("HTTP server started");
}

void handleWeb()
{
  static bool webServerStarted = false;

  // check for connection
  if (WiFi.status() == WL_CONNECTED)
  {
    if (!webServerStarted)
    {
      // turn off the board's LED when connected to wifi
      digitalWrite(led, 1);
      Serial.println();
      Serial.println("WiFi connected");
      Serial.print("IP address: ");
      Serial.println(WiFi.localIP());
      webServerStarted = true;
      setupWeb();
    }
    //    webServer.handleClient();
  }
  else
  {
    // blink the board's LED while connecting to wifi
    static uint8_t ledState = 0;
    EVERY_N_MILLIS(125)
    {
      ledState = ledState == 0 ? 1 : 0;
      digitalWrite(led, ledState);
      Serial.print(".");
    }
  }
}
