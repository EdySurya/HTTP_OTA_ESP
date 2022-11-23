#include <Arduino.h>
#include <ArduinoJson.h>

typedef struct
{
    /* data */
    uint32_t currentLength = 0;
    uint32_t totalLength;
    String currentFSVers = "0.1";
    String currentFirmVers = "0.1";
    uint8_t httpCode;
    uint32_t len;
} FS_Struct;

typedef struct
{
    const char *urlFirm;
    const char *urlFS;
    const char *urlUpdate;
    const char *Param_UrlFS = "UrlFS";
    const char *Param_UrlFirm = "UrlFirm";
    const char *Param_WiFiSSID = "WiFiSSID";
    const char *Param_WiFiPass = "WiFiPass";
    const char *ssid;
    const char *pass;
    const char *MacAddr;
} Firm_Dasar_Struct;

DynamicJsonDocument doc(64);
DynamicJsonDocument docVer(64);