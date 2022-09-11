

#ifndef BT_HOME_H_
    #define BT_HOME_H

    #include <stdint.h>

const uint16_t UNENCRYPTED_SERVICE_UUID = 0x181c;

enum OBJECT_FORMAT
{
    UNSIGNED_INT = 0b000,
    SIGNED_INT   = 0b001,
    FLOAT        = 0b010,
    STRING       = 0b011,
    MAC          = 0b100
};

const uint16_t OBJECT_FORMAT_SHIFT = 5;

enum DATA_TYPE
{
    BATTERY     = 0x01,
    TEMPERATURE = 0x02,
    HUMIDITY    = 0x03,
};

int32_t temperature(float const temperature, uint8_t dest[], uint32_t destLen);

int32_t humidity(float const humidity, uint8_t dest[], uint32_t destLen);

int32_t battery(float const batteryPercent, uint8_t dest[], uint32_t destLen);

#endif