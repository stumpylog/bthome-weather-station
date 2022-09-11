
#include "bthome.hpp"

using namespace std;
using namespace bthome::constants;

namespace bthome {

namespace encode {

int32_t _write_object_info(OBJECT_FORMAT const objectType, DATA_TYPE const dataType, size_t const dataLen, uint8_t dest[], size_t const destLen)
{
    int32_t bytesWritten = -1;

    // Plus 1 to account for the data type byte
    dest[0] = static_cast<uint8_t>((static_cast<uint8_t>(objectType) << OBJECT_FORMAT_SHIFT) | (dataLen + 1));
    bytesWritten = 1;

    dest[1] = static_cast<uint8_t>(dataType);
    bytesWritten++;

    return bytesWritten;
}

int32_t _write_data_bytes(uint16_t const data, uint8_t dest[], uint8_t destLen)
{
    dest[0] = static_cast<uint8_t>(data & 0xff);

    dest[1] = static_cast<uint8_t>((data >> 8) & 0xff);

    return 2;
}

int32_t temperature(float const temperature, uint8_t dest[], size_t const destLen)
{
    int32_t bytesWritten = -1;

    int16_t const temp_val = static_cast<int16_t>(100.0 * temperature);

    bytesWritten = _write_object_info(OBJECT_FORMAT::FLOAT, DATA_TYPE::TEMPERATURE, 2, dest, destLen);

    bytesWritten += _write_data_bytes(temp_val, &dest[2], destLen - bytesWritten);

    return bytesWritten;
}

int32_t humidity(float const humidity, uint8_t dest[], std::size_t destLen)
{
    int32_t bytesWritten = -1;

    uint16_t const scaled_humidity = static_cast<uint16_t>((10000 * humidity) / UINT16_MAX);

    bytesWritten = _write_object_info(OBJECT_FORMAT::UNSIGNED_INT, DATA_TYPE::HUMIDITY, 2, dest, destLen);

    bytesWritten += _write_data_bytes(scaled_humidity, &dest[2], destLen - bytesWritten);

    return bytesWritten;
}

int32_t battery(float const batteryPercent, uint8_t dest[], std::size_t destLen)
{
    int32_t bytesWritten = -1;

    uint16_t const scaled_battery = static_cast<uint16_t>(batteryPercent);

    bytesWritten = _write_object_info(OBJECT_FORMAT::UNSIGNED_INT, DATA_TYPE::BATTERY, 2, dest, destLen);

    bytesWritten += _write_data_bytes(scaled_battery, &dest[2], destLen - bytesWritten);

    return bytesWritten;
}

};

};