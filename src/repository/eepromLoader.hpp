#include <EEPROM.h>

void eepromLoader()
{
    EEPROM.begin(512);
    EEPROM.get(0, gBaseFirmwareStruct);
    EEPROM.end();

    Serial.printf("\nload ssid : %s", gBaseFirmwareStruct.ssid);
    Serial.printf("\nload pass : %s", gBaseFirmwareStruct.pass);
    Serial.printf("\nload state : %d", gBaseFirmwareStruct.state);
    Serial.printf("\nload fsUrl : %s", gBaseFirmwareStruct.fsUrl);
    Serial.printf("\nload firmwareUrl : %s", gBaseFirmwareStruct.firmwareUrl);
}

void eepromCommit(bool resetStatus)
{
    EEPROM.begin(512);

    Serial.printf("\ncommit ssid : %s", gBaseFirmwareStruct.ssid);
    Serial.printf("\ncommit pass : %s", gBaseFirmwareStruct.pass);
    Serial.printf("\ncommit state : %d", gBaseFirmwareStruct.state);
    Serial.printf("\ncommit fsUrl : %s", gBaseFirmwareStruct.fsUrl);
    Serial.printf("\ncommit firmareUrl : %s", gBaseFirmwareStruct.firmwareUrl);
    EEPROM.put(0, gBaseFirmwareStruct);
    EEPROM.commit();
    EEPROM.end();
    if (resetStatus)
        ESP.restart();
}

void dummyEeprom(bool resetStatus)
{
    EEPROM.begin(512);

    gBaseFirmwareStruct.state = true;
    sprintf(gBaseFirmwareStruct.ssid, "%s", "Signature Smart Hotel");
    sprintf(gBaseFirmwareStruct.pass, "%s", "smarthotelbali");
    sprintf(gBaseFirmwareStruct.fsUrl, "%s", "");
    sprintf(gBaseFirmwareStruct.firmwareUrl, "%s", "https://raw.githubusercontent.com/ahmaddidiks/ota_test/master/firmware.bin"); // https: // github.com/ahmaddidiks/ota_test/blob/master/firmware.bin
    Serial.printf("\ncommit ssid : %s", gBaseFirmwareStruct.ssid);
    Serial.printf("\ncommit pass : %s", gBaseFirmwareStruct.pass);
    Serial.printf("\ncommit state : %d", gBaseFirmwareStruct.state);
    Serial.printf("\ncommit fsUrl : %s", gBaseFirmwareStruct.fsUrl);
    Serial.printf("\ncommit firmareUrl : %s", gBaseFirmwareStruct.firmwareUrl);
    EEPROM.put(0, gBaseFirmwareStruct);
    EEPROM.commit();
    EEPROM.end();
    if (resetStatus)
        ESP.restart();
}
