
#include "bthome.h"

static const uint16_t OBJECT_FORMAT_SHIFT = 5;

int32_t _write_object_info(enum OBJECT_FORMAT const objectType, enum DATA_TYPE const dataType, uint32_t const dataLen,
                           uint8_t dest[], uint32_t const destLen)
{
    int32_t bytesWritten = -1;

    // Plus 1 to account for the data type byte
    dest[0]      = (objectType << OBJECT_FORMAT_SHIFT) | (dataLen + 1);
    bytesWritten = 1;

    dest[1] = dataType;
    bytesWritten++;

    return bytesWritten;
}

int32_t _write_data_bytes(uint16_t const data, uint8_t dest[], uint8_t destLen)
{
    dest[0] = data & 0xff;

    dest[1] = (data >> 8) & 0xff;

    return 2;
}

int32_t temperature(float const temperature, uint8_t dest[], uint32_t const destLen)
{
    int32_t bytesWritten = -1;

    int16_t const temp_val = (int16_t)(100.0 * temperature);

    bytesWritten = _write_object_info(FLOAT, TEMPERATURE, 2, dest, destLen);

    bytesWritten += _write_data_bytes(temp_val, &dest[2], destLen - bytesWritten);

    return bytesWritten;
}

int32_t humidity(float const humidity, uint8_t dest[], uint32_t destLen)
{
    int32_t bytesWritten = -1;

    uint16_t const scaled_humidity = (uint16_t)((10000 * humidity) / UINT16_MAX);

    bytesWritten = _write_object_info(UNSIGNED_INT, HUMIDITY, 2, dest, destLen);

    bytesWritten += _write_data_bytes(scaled_humidity, &dest[2], destLen - bytesWritten);

    return bytesWritten;
}

int32_t battery(float const batteryPercent, uint8_t dest[], uint32_t destLen)
{
    int32_t bytesWritten = -1;

    uint16_t const scaled_battery = (uint16_t)(batteryPercent);

    bytesWritten = _write_object_info(UNSIGNED_INT, BATTERY, 2, dest, destLen);

    bytesWritten += _write_data_bytes(scaled_battery, &dest[2], destLen - bytesWritten);

    return bytesWritten;
}
