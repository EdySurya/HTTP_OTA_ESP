#include <Arduino.h>

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
