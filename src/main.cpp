#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Update.h>
#include <LittleFS.h>
#include <ArduinoJson.h>

// const char* ssid = "Signature Smart Hotel";
// const char* password =  "smarthotelbali";
const char *ssid = "Signature Smart Hotel";
const char *password = "smarthotelbali";
const char *urlJson = "https://raw.githubusercontent.com/EdySurya/HTTP_OTA_ESP/LittleFS/data/CurrentVersion.json";
const char *url = "https://raw.githubusercontent.com/EdySurya/HTTP_OTA_ESP/LittleFS/src/Update/littlefs.bin";
const char *urlFirm = "https://raw.githubusercontent.com/EdySurya/HTTP_OTA_ESP/LittleFS/src/Update/firmware.bin";

String currentFSVers = "0.1";
String currentFirmVers = "0.1";
// void memulai SPIFFS
void initLittleFS()
{
  if (!LittleFS.begin(true))
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}
DynamicJsonDocument docVer(2048);
int currentLength = 0;
int totalLength;

// Function to update firmware incrementally
// Buffer is declared to be 128 so chunks of 128 bytes
// from firmware is written to device until server closes
void updateFirmware(uint8_t *data, size_t len)
{
  Update.write(data, len);
  currentLength += len;
  // Print dots while waiting for update to finish
  Serial.print('.');
  // if current length of written firmware is not equal to total firmware size, repeat
  if (currentLength != totalLength)
    return;
  Update.end(true);
  Serial.printf("\nUpdate Success, Total Size: %u", currentLength);
  // Restart ESP32 to see changes
  // ESP.restart();
}
void UpdateFS()
{
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  // configure server and url
  http.begin(url);
  // http.begin("192.168.1.12", 80, "/test.html");
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS, -1, LOW, NULL);
    if (httpCode == HTTP_CODE_OK)
    {
      // get length of document (is -1 when Server sends no Content-Length header)
      totalLength = http.getSize();
      int len = totalLength;
      // create buffer for read
      uint8_t buff[128] = {0};
      // get tcp stream
      WiFiClient *stream = http.getStreamPtr();
      // read all data from server
      while (http.connected() && (len > 0 || len == -1))
      {
        // get available data size
        size_t size = stream->available();
        if (size)
        {
          // read up to 128 byte
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          // write it to Serial
          updateFirmware(buff, c);
          // Serial.write(buff, c);
          if (len > 0)
          {
            len -= c;
          }
        }
        delay(1);
      }
      Serial.println();
      Serial.print("[HTTP] connection closed or file end.\n");
    }
  }
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void UpdateFirm()
{
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  // configure server and url
  http.begin(urlFirm);
  // http.begin("192.168.1.12", 80, "/test.html");
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  if (httpCode > 0)
  {
    // HTTP header has been send and Server response header has been handled
    Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    // file found at server
    Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH, -1, LOW, NULL);
    if (httpCode == HTTP_CODE_OK)
    {
      // get length of document (is -1 when Server sends no Content-Length header)
      totalLength = http.getSize();
      int len = totalLength;
      // create buffer for read
      uint8_t buff[128] = {0};
      // get tcp stream
      WiFiClient *stream = http.getStreamPtr();
      // read all data from server
      while (http.connected() && (len > 0 || len == -1))
      {
        // get available data size
        size_t size = stream->available();
        if (size)
        {
          // read up to 128 byte
          int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
          // write it to Serial
          updateFirmware(buff, c);
          // Serial.write(buff, c);
          if (len > 0)
          {
            len -= c;
          }
        }
        delay(1);
      }
      Serial.println();
      Serial.print("[HTTP] connection closed or file end.\n");
      Serial.println("Rebooting...");
      ESP.restart();
    }
  }
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
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
void setup()
{
  Serial.begin(115200);
  // delay(4000);
  WiFi.begin(ssid, password);
  // wifiMulti.addAP(ssid, password);

  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(250);
  }
  Serial.println("Connected to the WiFi network");
  initLittleFS();
  // LittleFS.format();
  readFile("/LampState.json", "r");
  readFile("/text.txt", "r");
  readFile("/index.html", "r");
  readFile("/script.js", "r");
  readFile("/style.css", "r");
  readFile("/CurrentVersion.json", "r");
  File file = LittleFS.open("/CurrentVersion.json", "r");
  if (file.available())
  {
    /* code */
    deserializeJson(docVer, file);
    currentFirmVers = docVer["FirmVers"].as<String>();
    currentFSVers = docVer["FSVers"].as<String>();
    Serial.println("Firmware Current Version :" + docVer["FirmVers"].as<String>());
    Serial.println("FS Current Version :" + docVer["FSVers"].as<String>());
  }
  file.close();

  // UpdateFS();       // jalankan update kalau pada data json current FS version berbeda dari sebelumnya
  // readFile("/LampState.json", "r");
  // readFile("/text.txt", "r");
  // readFile("/index.html", "r");
  // readFile("/script.js", "r");
  // readFile("/style.css", "r");
}
void loop()
{
  Serial.println("Firmware Current Version :" + currentFirmVers);
  Serial.println("FS Current Version :" + currentFSVers);
  DynamicJsonDocument doc(64);
  HTTPClient http;
  Serial.print("[HTTP] begin...\n");
  // configure server and url
  http.useHTTP10(true);
  http.begin(urlJson);
  // http.begin("192.168.1.12", 80, "/test.html");
  Serial.print("[HTTP] GET...\n");
  // start connection and send HTTP header
  int httpCode = http.GET();
  if (httpCode > 0)
  {
    deserializeJson(doc, http.getStream());
    Serial.print("FS Version : ");
    Serial.println(doc["FSVers"].as<String>());
    Serial.print("Firmware Version : ");
    Serial.println(doc["FirmVers"].as<String>());
    if (doc["FSVers"].as<String>() != currentFSVers)
    {
      currentFSVers = doc["FSVers"].as<String>();
      Serial.println("Memulai Update Versi baru dari Little FS");
      UpdateFS();
      // currentFSVers = docVer["FSVers"].as<String>();
    }
    else if (doc["FirmVers"].as<String>() != currentFirmVers)
    {
      currentFirmVers = doc["FirmVers"].as<String>();
      Serial.println("Memulai Update Versi baru Firmware");
      UpdateFirm();
      // currentFirmVers = docVer["FirmVers"].as<String>();
    }
    readFile("/LampState.json", "r");
    readFile("/text.txt", "r");
    // readFile("/index.html", "r");
    // readFile("/script.js", "r");
    // readFile("/style.css", "r");
    readFile("/CurrentVersion.json", "r");
  }
  else
  {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();

  delay(250);
}