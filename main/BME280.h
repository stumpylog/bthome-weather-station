
#ifndef _BME_280_H_
#define _BME_280_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "driver/i2c.h"

namespace sensors {
    class BME280 {
        public:
            BME280(i2c_port_t i2cDriver);
            ~BME280(void);

            float readTemperature(void);
            float readPressure(void);
            float readHumidity(void);

            void setSleepMode(void);

        private:

            typedef struct BME280CalibrationData {
            uint16_t dig_T1; ///< temperature compensation value
            int16_t dig_T2;  ///< temperature compensation value
            int16_t dig_T3;  ///< temperature compensation value

            uint16_t dig_P1; ///< pressure compensation value
            int16_t dig_P2;  ///< pressure compensation value
            int16_t dig_P3;  ///< pressure compensation value
            int16_t dig_P4;  ///< pressure compensation value
            int16_t dig_P5;  ///< pressure compensation value
            int16_t dig_P6;  ///< pressure compensation value
            int16_t dig_P7;  ///< pressure compensation value
            int16_t dig_P8;  ///< pressure compensation value
            int16_t dig_P9;  ///< pressure compensation value

            uint8_t dig_H1; ///< humidity compensation value
            int16_t dig_H2; ///< humidity compensation value
            uint8_t dig_H3; ///< humidity compensation value
            int16_t dig_H4; ///< humidity compensation value
            int16_t dig_H5; ///< humidity compensation value
            int8_t dig_H6;  ///< humidity compensation value
            } BME280CalibrationData;

            enum class REGISTER : uint8_t {
                DIG_T1 = 0x88,
                DIG_T2 = 0x8A,
                DIG_T3 = 0x8C,

                DIG_P1 = 0x8E,
                DIG_P2 = 0x90,
                DIG_P3 = 0x92,
                DIG_P4 = 0x94,
                DIG_P5 = 0x96,
                DIG_P6 = 0x98,
                DIG_P7 = 0x9A,
                DIG_P8 = 0x9C,
                DIG_P9 = 0x9E,

                DIG_H1 = 0xA1,
                DIG_H2 = 0xE1,
                DIG_H3 = 0xE3,
                DIG_H4 = 0xE4,
                DIG_H5 = 0xE5,
                DIG_H6 = 0xE7,

                CHIPID = 0xD0,
                VERSION = 0xD1,
                SOFTRESET = 0xE0,

                CAL26 = 0xE1, // R calibration stored in 0xE1-0xF0

                CONTROLHUMID = 0xF2,
                STATUS = 0XF3,
                CONTROL = 0xF4,
                CONFIG = 0xF5,
                PRESSUREDATA = 0xF7,
                TEMPDATA = 0xFA,
                HUMIDDATA = 0xFD
                };

                enum class SAMPLING_RATE: uint8_t {
                    SAMPLING_NONE = 0b000,
                    SAMPLING_X1 = 0b001,
                    SAMPLING_X2 = 0b010,
                    SAMPLING_X4 = 0b011,
                    SAMPLING_X8 = 0b100,
                    SAMPLING_X16 = 0b101
                };

                 enum class SENSOR_MODE: uint8_t {
    MODE_SLEEP = 0b00,
    MODE_FORCED = 0b01,
    MODE_NORMAL = 0b11
  };

    enum class SENSOR_FILTER: uint8_t {
    FILTER_OFF = 0b000,
    FILTER_X2 = 0b001,
    FILTER_X4 = 0b010,
    FILTER_X8 = 0b011,
    FILTER_X16 = 0b100
  };

    enum class STANDBY_DURATION : uint8_t {
    STANDBY_MS_0_5 = 0b000,
    STANDBY_MS_10 = 0b110,
    STANDBY_MS_20 = 0b111,
    STANDBY_MS_62_5 = 0b001,
    STANDBY_MS_125 = 0b010,
    STANDBY_MS_250 = 0b011,
    STANDBY_MS_500 = 0b100,
    STANDBY_MS_1000 = 0b101
  };

            void write8(REGISTER reg, uint8_t value);
            uint8_t read8(REGISTER reg);
            uint16_t read16(REGISTER reg);
            uint32_t read24(REGISTER reg);
            int16_t readS16(REGISTER reg);
            uint16_t read16_LE(REGISTER reg); // little endian
            int16_t readS16_LE(REGISTER reg); // little endian

            constexpr uint8_t BME280_ADDRESS{0x77};

            i2c_port_t _i2cDriver;
            BME280CalibrationData _calibrationData;
            int32_t t_fine;

    };
}

#ifdef __cplusplus
}
#endif

#endif