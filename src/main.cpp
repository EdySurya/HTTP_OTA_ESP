#include <WiFi.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
#include <WiFiClientSecure.h>

#include <service/FSUpdate.hpp>
#include <config/mainStruct.hpp>

baseFirmwareStruct gBaseFirmwareStruct;

#include <repository/eepromLoader.hpp>

// SKETCH BEGIN

void setup()
{
  Serial.begin(115200);
  delay(100);

  // dummyEeprom(false);
  
  // delay(100000000);

  eepromLoader();

  Serial.printf("state: %d", gBaseFirmwareStruct.state);

  if (gBaseFirmwareStruct.state)
  {
    gBaseFirmwareStruct.state = false; // change EEPROM OTA state to false
    eepromCommit(false);

    delay(3000);
    firmwareUpdate(gBaseFirmwareStruct.ssid,
                   gBaseFirmwareStruct.pass,
                   gBaseFirmwareStruct.firmwareUrl,
                   gBaseFirmwareStruct.fsUrl);
  }
}

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

AsyncWebServer server(80);
AsyncEventSource events("/events");

IPAddress local_IP(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
void loop()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  Serial.setDebugOutput(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(local_IP, gateway, subnet);
  const char *ssid = WiFi.macAddress().c_str();
  const char *pass = "Digitels123456";
  WiFi.softAP(ssid, pass);

  events.onConnect([](AsyncEventSourceClient *client)
                   { client->send("hello!", NULL, millis(), 1000); });
  server.addHandler(&events);

  server.on("/", [](AsyncWebServerRequest *request)
            { 
              AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/index.html", "", false);
              request->send(response); });
  server.on("/style.css", [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/style.css", "text/css"); });
  server.on("/script.js", [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/script.js", "text/js"); });
  server.on("/submit", HTTP_GET, [](AsyncWebServerRequest *request)
            {
              String message;

              sprintf(gBaseFirmwareStruct.fsUrl , request->getParam("UrlFS")->value().c_str());
              sprintf(gBaseFirmwareStruct.firmwareUrl , request->getParam("UrlFirm")->value().c_str());
              sprintf(gBaseFirmwareStruct.ssid , request->getParam("WiFiSSID")->value().c_str());
              sprintf(gBaseFirmwareStruct.pass , request->getParam("WiFiPass")->value().c_str());
              Serial.println(gBaseFirmwareStruct.firmwareUrl);
              Serial.println(gBaseFirmwareStruct.fsUrl);
              Serial.println(gBaseFirmwareStruct.ssid);
              Serial.println(gBaseFirmwareStruct.pass);
              
              message = " Credentials received by ESP board!!! ";
              request->send(200, "text/plain", message);
              
              if ((strcmp(gBaseFirmwareStruct.fsUrl,       "#") != 0) ||\
                  (strcmp(gBaseFirmwareStruct.firmwareUrl, "#") != 0) ||\
                  (strcmp(gBaseFirmwareStruct.ssid,        "#") != 0) ||\
                  (strcmp(gBaseFirmwareStruct.pass,        "#") != 0))
              {
                gBaseFirmwareStruct.state = true;
                // commit EEPROM then restart
                eepromCommit(true);
              } });
  server.begin();

  while (1)
  {
  }
}
