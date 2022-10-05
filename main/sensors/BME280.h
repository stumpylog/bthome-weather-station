
#ifndef _BME_280_H_
#define _BME_280_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "driver/i2c.h"
#include "i2cBus.h"

#include <cstdint>

namespace sensors
{
    class BME280
    {
      public:
        BME280(i2cBus const& bus);
        ~BME280(void);

        void readData(void);

        void setSleepMode(void);

        float temperature;
        float pressure;
        float humidity;

      private:
        void readCalibration(void);
        float calculateTemperature(int32_t);
        float calculatePressure(int32_t);
        float calculateHumidity(int32_t);

        typedef struct BME280CalibrationData
        {
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

        enum class REGISTER : uint8_t
        {
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

            CHIPID    = 0xD0,
            VERSION   = 0xD1,
            SOFTRESET = 0xE0,

            CAL26 = 0xE1, // R calibration stored in 0xE1-0xF0

            CONTROLHUMID = 0xF2,
            STATUS       = 0XF3,
            CONTROL      = 0xF4,
            CONFIG       = 0xF5,
            PRESSUREDATA = 0xF7,
            TEMPDATA     = 0xFA,
            HUMIDDATA    = 0xFD
        };

        enum class SAMPLING_RATE : uint8_t
        {
            SAMPLING_NONE = 0b000,
            SAMPLING_X1   = 0b001,
            SAMPLING_X2   = 0b010,
            SAMPLING_X4   = 0b011,
            SAMPLING_X8   = 0b100,
            SAMPLING_X16  = 0b101
        };

        enum class SENSOR_MODE : uint8_t
        {
            SLEEP  = 0b00,
            FORCED = 0b01,
            NORMAL = 0b11
        };

        enum class SENSOR_FILTER : uint8_t
        {
            FILTER_OFF = 0b000,
            FILTER_X2  = 0b001,
            FILTER_X4  = 0b010,
            FILTER_X8  = 0b011,
            FILTER_X16 = 0b100
        };

        enum class STANDBY_DURATION : uint8_t
        {
            STANDBY_MS_0_5  = 0b000,
            STANDBY_MS_10   = 0b110,
            STANDBY_MS_20   = 0b111,
            STANDBY_MS_62_5 = 0b001,
            STANDBY_MS_125  = 0b010,
            STANDBY_MS_250  = 0b011,
            STANDBY_MS_500  = 0b100,
            STANDBY_MS_1000 = 0b101
        };

        /**************************************************************************/
        /*!
            @brief  config register
        */
        /**************************************************************************/
        struct config
        {
            // inactive duration (standby time) in normal mode
            // 000 = 0.5 ms
            // 001 = 62.5 ms
            // 010 = 125 ms
            // 011 = 250 ms
            // 100 = 500 ms
            // 101 = 1000 ms
            // 110 = 10 ms
            // 111 = 20 ms
            unsigned int t_sb : 3; ///< inactive duration (standby time) in normal mode

            // filter settings
            // 000 = filter off
            // 001 = 2x filter
            // 010 = 4x filter
            // 011 = 8x filter
            // 100 and above = 16x filter
            unsigned int filter : 3; ///< filter settings

            // unused - don't set
            unsigned int none : 1;     ///< unused - don't set
            unsigned int spi3w_en : 1; ///< unused - don't set

            /// @return combined config register
            unsigned int get()
            {
                return (t_sb << 5) | (filter << 2) | spi3w_en;
            }
        };
        config _configReg; //!< config register object

        /**************************************************************************/
        /*!
            @brief  ctrl_meas register
        */
        /**************************************************************************/
        struct ctrl_meas
        {
            // temperature oversampling
            // 000 = skipped
            // 001 = x1
            // 010 = x2
            // 011 = x4
            // 100 = x8
            // 101 and above = x16
            unsigned int osrs_t : 3; ///< temperature oversampling

            // pressure oversampling
            // 000 = skipped
            // 001 = x1
            // 010 = x2
            // 011 = x4
            // 100 = x8
            // 101 and above = x16
            unsigned int osrs_p : 3; ///< pressure oversampling

            // device mode
            // 00       = sleep
            // 01 or 10 = forced
            // 11       = normal
            unsigned int mode : 2; ///< device mode

            /// @return combined ctrl register
            unsigned int get()
            {
                return (osrs_t << 5) | (osrs_p << 2) | mode;
            }
        };
        ctrl_meas _measReg; //!< measurement register object

        /**************************************************************************/
        /*!
            @brief  ctrl_hum register
        */
        /**************************************************************************/
        struct ctrl_hum
        {
            /// unused - don't set
            unsigned int none : 5;

            // pressure oversampling
            // 000 = skipped
            // 001 = x1
            // 010 = x2
            // 011 = x4
            // 100 = x8
            // 101 and above = x16
            unsigned int osrs_h : 3; ///< pressure oversampling

            /// @return combined ctrl hum register
            unsigned int get()
            {
                return (osrs_h);
            }
        };
        ctrl_hum _humReg; //!< hum register object

        static constexpr uint8_t BME280_ADDRESS {0x77};

        i2cBus const& m_bus;
        BME280CalibrationData _calibrationData;
        int32_t t_fine;
    };
} // namespace sensors

#ifdef __cplusplus
}
#endif

#endif
