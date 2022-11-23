#include <config/mainStruct.hpp>
#include <WiFi.h>
#include <LittleFS.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");
Firm_Dasar_Struct FirmDasar;

void initLittleFS()
{
    if (!LittleFS.begin(true))
    {
        Serial.println("An error has occurred while mounting SPIFFS");
    }
    Serial.println("SPIFFS mounted successfully");
}

void readFile(const char *path, const char *mode)
{
    File file = LittleFS.open(path, mode);
    while (file.available())
    {
        Serial.write(file.read());
    }
    Serial.println("");
    file.close();
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type,
             void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        break;
    case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
    case WS_EVT_DATA:
    case WS_EVT_PONG:
    case WS_EVT_ERROR:
        break;
    }
}

void notFound(AsyncWebServerRequest *request)
{
    request->send(404, "text/plain", "Not found");
}

void serverHandle()
{
    server.on("/", [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/index ori.html", "text/html", false); });
    server.on("/style.css", [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/style ori.css", "text/css"); });
    server.on("/script.js", [](AsyncWebServerRequest *request)
              { request->send(LittleFS, "/script ori.js", "text/js"); });
    server.on("/submit", HTTP_GET, [](AsyncWebServerRequest *request)
              {
    String message;
    FirmDasar.urlFS = request->getParam(FirmDasar.Param_UrlFS)->value().c_str();
    FirmDasar.urlFirm = request->getParam(FirmDasar.Param_UrlFirm)->value().c_str();
    FirmDasar.ssid = request->getParam(FirmDasar.Param_WiFiSSID)->value().c_str();
    FirmDasar.pass = request->getParam(FirmDasar.Param_WiFiPass)->value().c_str();
    // ESP.restart();
    Serial.println(String(FirmDasar.urlFS));
    Serial.println(String(FirmDasar.urlFirm));
    Serial.println(String(FirmDasar.ssid));
    Serial.println(String(FirmDasar.pass));
    message = " Credentials received by ESP board!!! ";
    request->send(200, "text/plain", message); });
    server.onNotFound(notFound);
    server.begin();
    server.serveStatic("/", LittleFS, "/");
    // Start server
    server.begin();
}
