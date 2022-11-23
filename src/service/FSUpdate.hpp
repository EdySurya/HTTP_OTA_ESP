#include <HTTPClient.h>
#include <WiFi.h>
#include <Update.h>
#include <service/FirmwareDasar.hpp>

FS_Struct my_Struct;
void UpdateFS()
{
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin(FirmDasar.urlFS);
    // http.begin("192.168.1.12", 80, "/test.html");
    Serial.print("[HTTP] GET...\n");
    my_Struct.httpCode = http.GET();
    if (my_Struct.httpCode > 0)
    {
        Serial.printf("[HTTP] GET... code: %d\n", my_Struct.httpCode);
        Update.begin(UPDATE_SIZE_UNKNOWN, U_SPIFFS, -1, LOW, NULL);
        if (my_Struct.httpCode == HTTP_CODE_OK)
        {
            // get length of document (is -1 when Server sends no Content-Length header)
            my_Struct.totalLength = http.getSize();
            my_Struct.len = my_Struct.totalLength;
            uint8_t buff[128] = {0};
            WiFiClient *stream = http.getStreamPtr();
            while (http.connected() && (my_Struct.len > 0 || my_Struct.len == -1))
            {
                size_t size = stream->available();
                if (size)
                {
                    uint8_t c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                    Update.write(buff, c);
                    my_Struct.currentLength += c;
                    Serial.print('.');
                    if (my_Struct.currentLength != my_Struct.totalLength)
                        return;
                    Update.end(true);
                    Serial.printf("\nUpdate Success, Total Size: %u", my_Struct.currentLength);
                    if (my_Struct.len > 0)
                    {
                        my_Struct.len -= c;
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
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(my_Struct.httpCode).c_str());
    }
    http.end();
}

void UpdateFirm()
{
    HTTPClient http;
    Serial.print("[HTTP] begin...\n");
    http.begin(FirmDasar.urlFirm);
    Serial.print("[HTTP] GET...\n");
    my_Struct.httpCode = http.GET();
    if (my_Struct.httpCode > 0)
    {
        Serial.printf("[HTTP] GET... code: %d\n", my_Struct.httpCode);
        Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH, -1, LOW, NULL);
        if (my_Struct.httpCode == HTTP_CODE_OK)
        {
            my_Struct.totalLength = http.getSize();
            my_Struct.len = my_Struct.totalLength;
            uint8_t buff[128] = {0};
            WiFiClient *stream = http.getStreamPtr();
            while (http.connected() && (my_Struct.len > 0 || my_Struct.len == -1))
            {
                size_t size = stream->available();
                if (size)
                {
                    uint8_t c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));
                    Update.write(buff, c);
                    my_Struct.currentLength += c;
                    Serial.print('.');
                    if (my_Struct.currentLength != my_Struct.totalLength)
                        return;
                    Update.end(true);
                    Serial.printf("\nUpdate Success, Total Size: %u", my_Struct.currentLength);
                    if (my_Struct.len > 0)
                    {
                        my_Struct.len -= c;
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
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(my_Struct.httpCode).c_str());
    }
    http.end();
    ESP.restart();
}