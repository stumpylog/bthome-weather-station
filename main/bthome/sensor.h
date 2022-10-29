

#ifndef BT_HOME_SENSOR_H_
#define BT_HOME_SENSOR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "constants.h"

#include <cstdint>

namespace bthome
{

    namespace sensor
    {

        class Sensor
        {
          public:
            Sensor(enum bthome::constants::OBJECT_FORMAT objectType, enum constants::DATA_TYPE dataType,
                   float const scaleFactor, float const data);
            Sensor(enum constants::OBJECT_FORMAT objectType, enum constants::DATA_TYPE dataType, uint64_t const data);
            ~Sensor(void);
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

        class UnsignedIntSensor : public Sensor
        {
          public:
            UnsignedIntSensor(enum constants::DATA_TYPE dataType, uint64_t const data);
            UnsignedIntSensor(enum constants::DATA_TYPE dataType, float const scaleFactor, float const data);
        };

        class SignedIntSensor : public Sensor
        {
          public:
            SignedIntSensor(enum constants::DATA_TYPE dataType, uint64_t const data);
            SignedIntSensor(enum constants::DATA_TYPE dataType, float const scaleFactor, float const data);
        };

        class PacketIdIdSensor : public UnsignedIntSensor
        {
          public:
            PacketIdIdSensor(uint64_t const data);
        };

        class TemperatureSensor : public SignedIntSensor
        {
          public:
            TemperatureSensor(float const data);
        };

        class HumiditySensor : public UnsignedIntSensor
        {
          public:
            HumiditySensor(float const data);
        };

        class PressureSensor : public UnsignedIntSensor
        {
          public:
            PressureSensor(float const data);
        };

    }; // namespace sensor

}; // namespace bthome

#ifdef __cplusplus
}
#endif

#endif
