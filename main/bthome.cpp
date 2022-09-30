
#ifdef __cplusplus
extern "C" {
#endif

#include "bthome.h"

#include "esp_log.h"

#include <cstring>

#define NAME "bthome"

namespace bthome
{

    namespace encode
    {

        BTHomeSensor::BTHomeSensor(enum constants::OBJECT_FORMAT objectType, enum constants::DATA_TYPE dataType,
                                   float const scaleFactor, float const data)
            : m_objectType(objectType), m_dataType(dataType), m_scale {scaleFactor}, m_scaledData {0}, m_payloadIdx {0}
        {
            this->m_scaledData = static_cast<uint64_t>(data * this->m_scale);
            this->writeObjectInfo();
            this->writeObjectData();
        }

        BTHomeSensor::BTHomeSensor(enum constants::OBJECT_FORMAT objectType, enum constants::DATA_TYPE dataType,
                                   uint64_t const data)
            : m_objectType(objectType), m_dataType(dataType), m_scale {0.0f}, m_scaledData {data}, m_payloadIdx {0}
        {
            this->writeObjectInfo();
            this->writeObjectData();
        }

        BTHomeSensor::~BTHomeSensor(void) { }

        const uint8_t* BTHomeSensor::getPayload(void) const
        {
            return &this->m_payload[0];
        }

        uint32_t BTHomeSensor::getPayloadSize(void) const
        {
            return this->m_payloadIdx;
        }

        uint8_t BTHomeSensor::minBytes(int64_t scaledData) const
        {
            if (scaledData == (int8_t)(scaledData & 0xFF))
            {
                return 1;
            }
            if (scaledData == (int16_t)(scaledData & 0xFFFF))
            {
                return 2;
            }
            if (scaledData == (int32_t)(scaledData & 0xFFFFFF))
            {
                return 3;
            }
            if (scaledData == (int32_t)(scaledData & 0xFFFFFFFF))
            {
                return 4;
            }
            return 8;
        }

        void BTHomeSensor::writeObjectInfo(void)
        {
            static constexpr uint16_t OBJECT_FORMAT_SHIFT = 5;

            // Get the minimum number of bytes to store the scaled data in
            uint8_t const dataLen = this->minBytes(this->m_scaledData);

            // Ensure the payload is filled from the start
            this->m_payloadIdx = 0;

            // Plus 1 to account for the data type byte
            this->m_payload[this->m_payloadIdx] =
                (static_cast<uint8_t>(this->m_objectType) << OBJECT_FORMAT_SHIFT) | (dataLen + 1);
            this->m_payloadIdx++;

            m_payload[this->m_payloadIdx] = static_cast<uint8_t>(this->m_dataType);
            this->m_payloadIdx++;
        }

        void BTHomeSensor::writeObjectData(void)
        {
            // Get the minimum number of bytes to store the scaled data in
            uint8_t const dataLen = this->minBytes(this->m_scaledData);

            for (int8_t i = 0; i < dataLen; i++)
            {
                this->m_payload[this->m_payloadIdx] = static_cast<uint8_t>((this->m_scaledData >> (i * 8)) & 0xff);
                this->m_payloadIdx++;
            }
        }

        BTHomeUnsignedIntSensor::BTHomeUnsignedIntSensor(enum constants::DATA_TYPE dataType, float const scaleFactor,
                                                         float const data)
            : BTHomeSensor(constants::OBJECT_FORMAT::UNSIGNED_INT, dataType, scaleFactor, data) {};

        BTHomeUnsignedIntSensor::BTHomeUnsignedIntSensor(enum constants::DATA_TYPE dataType, uint64_t const data)
            : BTHomeSensor(constants::OBJECT_FORMAT::UNSIGNED_INT, dataType, data) {};

        BTHomeSignedIntSensor::BTHomeSignedIntSensor(enum constants::DATA_TYPE dataType, float const scaleFactor,
                                                     float const data)
            : BTHomeSensor(constants::OBJECT_FORMAT::SIGNED_INT, dataType, scaleFactor, data) {};

        BTHomeSignedIntSensor::BTHomeSignedIntSensor(enum constants::DATA_TYPE dataType, uint64_t const data)
            : BTHomeSensor(constants::OBJECT_FORMAT::SIGNED_INT, dataType, data) {};

        BTHomePacketIdIdSensor::BTHomePacketIdIdSensor(uint64_t const data)
            : BTHomeUnsignedIntSensor(constants::DATA_TYPE::PACKET_ID, data) {};

        BTHomeTemperatureSensor::BTHomeTemperatureSensor(float const data)
            : BTHomeSignedIntSensor(constants::DATA_TYPE::TEMPERATURE, 100.0f, data) {};

        BTHomeHumiditySensor::BTHomeHumiditySensor(float const data)
            : BTHomeUnsignedIntSensor(constants::DATA_TYPE::HUMIDITY, 100.0f, data) {};

        BTHomePressureSensor::BTHomePressureSensor(float const data)
            : BTHomeUnsignedIntSensor(constants::DATA_TYPE::PRESSURE, 100.0f, data) {};
    }; // namespace encode

}; // namespace bthome

#ifdef __cplusplus
}
#endif