
#ifdef __cplusplus
extern "C" {
#endif

#include "bthome.h"

namespace bthome
{

    namespace encode
    {

        static constexpr uint16_t OBJECT_FORMAT_SHIFT = 5;

        int32_t _write_object_info(enum constants::OBJECT_FORMAT const objectType,
                                   enum constants::DATA_TYPE const dataType, uint32_t const dataLen, uint8_t dest[],
                                   uint32_t const destLen)
        {
            int32_t bytesWritten = -1;

            // Plus 1 to account for the data type byte
            dest[0]      = (static_cast<uint8_t>(objectType) << OBJECT_FORMAT_SHIFT) | (dataLen + 1);
            bytesWritten = 1;

            dest[1] = static_cast<uint8_t>(dataType);
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

            int16_t const temp_val = static_cast<int16_t>(100.0 * temperature);

            bytesWritten = _write_object_info(constants::OBJECT_FORMAT::FLOAT, constants::DATA_TYPE::TEMPERATURE, 2,
                                              dest, destLen);

            bytesWritten += _write_data_bytes(temp_val, &dest[2], destLen - bytesWritten);

            return bytesWritten;
        }

        int32_t humidity(float const humidity, uint8_t dest[], uint32_t destLen)
        {
            int32_t bytesWritten = -1;

            uint16_t const scaled_humidity = static_cast<uint16_t>((10000 * humidity) / UINT16_MAX);

            bytesWritten = _write_object_info(constants::OBJECT_FORMAT::UNSIGNED_INT, constants::DATA_TYPE::HUMIDITY, 2,
                                              dest, destLen);

            bytesWritten += _write_data_bytes(scaled_humidity, &dest[2], destLen - bytesWritten);

            return bytesWritten;
        }

        int32_t battery(float const batteryPercent, uint8_t dest[], uint32_t destLen)
        {
            int32_t bytesWritten = -1;

            uint16_t const scaled_battery = static_cast<uint16_t>(batteryPercent);

            bytesWritten = _write_object_info(constants::OBJECT_FORMAT::UNSIGNED_INT, constants::DATA_TYPE::BATTERY, 2,
                                              dest, destLen);

            bytesWritten += _write_data_bytes(scaled_battery, &dest[2], destLen - bytesWritten);

            return bytesWritten;
        }

    }; // namespace encode

}; // namespace bthome

#ifdef __cplusplus
}
#endif