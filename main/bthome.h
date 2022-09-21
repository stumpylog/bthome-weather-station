

#ifndef BT_HOME_H_
#define BT_HOME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

namespace bthome
{

    namespace constants
    {

        constexpr uint16_t UNENCRYPTED_SERVICE_UUID = 0x181c;

        enum class OBJECT_FORMAT : uint8_t
        {
            UNSIGNED_INT = 0b000,
            SIGNED_INT   = 0b001,
            FLOAT        = 0b010,
            STRING       = 0b011,
            MAC          = 0b100
        };

        enum class DATA_TYPE : uint8_t
        {
            PACKET_ID   = 0x00,
            BATTERY     = 0x01,
            TEMPERATURE = 0x02,
            HUMIDITY    = 0x03,
            PRESSURE    = 0x04,
        };

    }; // namespace constants

    namespace encode
    {

        int32_t temperature(float const temperature, uint8_t dest[], uint32_t destLen);

        int32_t humidity(float const humidity, uint8_t dest[], uint32_t destLen);

        int32_t battery(float const batteryPercent, uint8_t dest[], uint32_t destLen);

        int32_t pressure(float const pressure, uint8_t dest[], uint32_t destLen);

        int32_t packet_id(uint8_t const packetId, uint8_t dest[], uint32_t destLen);

    }; // namespace encode

}; // namespace bthome

#ifdef __cplusplus
}
#endif

#endif