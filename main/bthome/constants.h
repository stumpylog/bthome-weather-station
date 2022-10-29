

#ifndef BT_HOME_CONSTANTS_H_
#define BT_HOME_CONSTANTS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

namespace bthome
{

    namespace constants
    {

        constexpr uint16_t UNENCRYPTED_SERVICE_UUID {0x181c};
        constexpr uint16_t ENCRYPTED_SERVICE_UUID {0x1E18};
        constexpr uint8_t BLE_ADVERT_MAX_LEN {31};
        constexpr uint8_t NONCE_LEN {12};
        constexpr uint8_t BIND_KEY_LEN {12};

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

        enum BLE_ADVERT_DATA_TYPE
        {
            TYPE          = 0x1,
            COMPLETE_NAME = 0x9,
            SERVICE_DATA  = 0x16
        };

        enum BLE_FLAGS_DATA_TYPE
        {
            GENERAL_DISCOVERY   = (0x01 << 1),
            BREDR_NOT_SUPPORTED = (0x01 << 2),
        };

    }; // namespace constants

}; // namespace bthome

#ifdef __cplusplus
}
#endif

#endif
