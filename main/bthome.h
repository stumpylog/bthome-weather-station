

#ifndef BT_HOME_H_
#define BT_HOME_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <cstdint>

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

        class BTHomeSensor
        {
          public:
            BTHomeSensor(enum constants::OBJECT_FORMAT objectType, enum constants::DATA_TYPE dataType,
                         float const scaleFactor, float const data);
            BTHomeSensor(enum constants::OBJECT_FORMAT objectType, enum constants::DATA_TYPE dataType,
                         uint64_t const data);
            ~BTHomeSensor(void);
            const uint8_t* getPayload(void) const;
            uint32_t getPayloadSize(void) const;

          protected:
            uint8_t minBytes(int64_t scaledData) const;
            void writeObjectInfo(void);
            void writeObjectData(void);

            enum constants::OBJECT_FORMAT m_objectType;
            enum constants::DATA_TYPE m_dataType;
            float m_scale;
            uint64_t m_scaledData;
            uint32_t m_payloadIdx;
            // Nothing is going to be larger than the max advertisement size
            uint8_t m_payload[32] {0};
        };

        class BTHomeUnsignedIntSensor : public BTHomeSensor
        {
          public:
            BTHomeUnsignedIntSensor(enum constants::DATA_TYPE dataType, uint64_t const data);
            BTHomeUnsignedIntSensor(enum constants::DATA_TYPE dataType, float const scaleFactor, float const data);
        };

        class BTHomeSignedIntSensor : public BTHomeSensor
        {
          public:
            BTHomeSignedIntSensor(enum constants::DATA_TYPE dataType, uint64_t const data);
            BTHomeSignedIntSensor(enum constants::DATA_TYPE dataType, float const scaleFactor, float const data);
        };

        class BTHomePacketIdIdSensor : public BTHomeUnsignedIntSensor
        {
          public:
            BTHomePacketIdIdSensor(uint64_t const data);
        };

        class BTHomeTemperatureSensor : public BTHomeSignedIntSensor
        {
          public:
            BTHomeTemperatureSensor(float const data);
        };

        class BTHomeHumiditySensor : public BTHomeUnsignedIntSensor
        {
          public:
            BTHomeHumiditySensor(float const data);
        };

        class BTHomePressureSensor : public BTHomeUnsignedIntSensor
        {
          public:
            BTHomePressureSensor(float const data);
        };

    }; // namespace encode

}; // namespace bthome

#ifdef __cplusplus
}
#endif

#endif