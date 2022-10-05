
#ifdef __cplusplus
extern "C" {
#endif
#include "BME280.h"

#include "driver/i2c.h"
#include "endian.h"
#include "esp_log.h"

#include <math.h>

#define NAME "bme280"

namespace sensors
{
    BME280::BME280(i2cBus const& bus) : m_bus(bus)
    {
        // Soft reset the sensor
        this->m_bus.write8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::SOFTRESET), 0xB6);

        // wait 10ms
        vTaskDelay(100 / portTICK_PERIOD_MS);

        // Wait to finish reading calibration
        uint8_t rStatus = this->m_bus.read8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::STATUS));
        while ((rStatus & (1 << 0)) != 0)
        {
            vTaskDelay(5 / portTICK_PERIOD_MS);
            rStatus = this->m_bus.read8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::STATUS));
        }

        // Read calibration data
        this->readCalibration();

        _measReg.mode   = static_cast<uint8_t>(SENSOR_MODE::NORMAL);
        _measReg.osrs_t = static_cast<uint8_t>(SAMPLING_RATE::SAMPLING_X16);
        _measReg.osrs_p = static_cast<uint8_t>(SAMPLING_RATE::SAMPLING_X16);

        _humReg.osrs_h      = static_cast<uint8_t>(SAMPLING_RATE::SAMPLING_X16);
        _configReg.filter   = static_cast<uint8_t>(SENSOR_FILTER::FILTER_OFF);
        _configReg.t_sb     = static_cast<uint8_t>(STANDBY_DURATION::STANDBY_MS_0_5);
        _configReg.spi3w_en = 0;

        // making sure sensor is in sleep mode before setting configuration
        // as it otherwise may be ignored
        this->m_bus.write8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::CONTROL),
                           static_cast<uint8_t>(SENSOR_MODE::SLEEP));

        // you must make sure to also set REGISTER_CONTROL after setting the
        // CONTROLHUMID register, otherwise the values won't be applied (see
        // DS 5.4.3)
        this->m_bus.write8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::CONTROLHUMID), _humReg.get());
        this->m_bus.write8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::CONFIG), _configReg.get());
        this->m_bus.write8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::CONTROL), _measReg.get());

        // Wait 100ms
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    BME280::~BME280(void) { }

    void BME280::readCalibration(void)
    {
        _calibrationData.dig_T1 = this->m_bus.read16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_T1));
        _calibrationData.dig_T2 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_T2));
        _calibrationData.dig_T3 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_T3));

        _calibrationData.dig_P1 = this->m_bus.read16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_P1));
        _calibrationData.dig_P2 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_P2));
        _calibrationData.dig_P3 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_P3));
        _calibrationData.dig_P4 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_P4));
        _calibrationData.dig_P5 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_P5));
        _calibrationData.dig_P6 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_P6));
        _calibrationData.dig_P7 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_P7));
        _calibrationData.dig_P8 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_P8));
        _calibrationData.dig_P9 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_P9));

        _calibrationData.dig_H1 = this->m_bus.read8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_H1));
        _calibrationData.dig_H2 = this->m_bus.readS16_LE(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_H2));
        _calibrationData.dig_H3 = this->m_bus.read8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_H3));
        _calibrationData.dig_H4 =
            ((int8_t)this->m_bus.read8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_H4)) << 4) |
            (this->m_bus.read8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_H5)) & 0xF);
        _calibrationData.dig_H5 =
            ((int8_t)this->m_bus.read8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_H6)) << 4) |
            (this->m_bus.read8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_H5)) >> 4);
        _calibrationData.dig_H6 = (int8_t)this->m_bus.read8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::DIG_H6));
    }

    void BME280::readData(void)
    {
        uint8_t buffer[8];
        this->m_bus.burstRead(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::PRESSUREDATA), 8, &buffer[0]);

        uint32_t rawPressure    = uint32_t(buffer[0]) << 16 | uint32_t(buffer[1]) << 8 | uint32_t(buffer[2]);
        uint32_t rawTemperature = uint32_t(buffer[3]) << 16 | uint32_t(buffer[4]) << 8 | uint32_t(buffer[5]);
        uint16_t rawHumidity    = uint32_t(buffer[6]) << 8 | uint32_t(buffer[7]);

        // Temperature compensates the other values, must be first
        this->temperature = this->calculateTemperature(rawTemperature);
        this->pressure    = this->calculatePressure(rawPressure);
        this->humidity    = this->calculateHumidity(rawHumidity);
    }

    float BME280::calculateTemperature(uint32_t adc_T)
    {
        int32_t var1, var2;

        // value in case temp measurement was disabled
        if (adc_T == 0x800000)
        {
            return NAN;
        }
        adc_T >>= 4;

        var1 = (int32_t)((adc_T / 8) - ((int32_t)_calibrationData.dig_T1 * 2));
        var1 = (var1 * ((int32_t)_calibrationData.dig_T2)) / 2048;
        var2 = (int32_t)((adc_T / 16) - ((int32_t)_calibrationData.dig_T1));
        var2 = (((var2 * var2) / 4096) * ((int32_t)_calibrationData.dig_T3)) / 16384;

        t_fine = var1 + var2;

        int32_t T = (t_fine * 5 + 128) / 256;

        return (float)T / 100;
    }

    float BME280::calculateHumidity(uint32_t adc_H)
    {
        int32_t var1, var2, var3, var4, var5;

        if (adc_H == 0x8000)
        {
            return NAN;
        }

        var1       = t_fine - ((int32_t)76800);
        var2       = (int32_t)(adc_H * 16384);
        var3       = (int32_t)(((int32_t)_calibrationData.dig_H4) * 1048576);
        var4       = ((int32_t)_calibrationData.dig_H5) * var1;
        var5       = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
        var2       = (var1 * ((int32_t)_calibrationData.dig_H6)) / 1024;
        var3       = (var1 * ((int32_t)_calibrationData.dig_H3)) / 2048;
        var4       = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
        var2       = ((var4 * ((int32_t)_calibrationData.dig_H2)) + 8192) / 16384;
        var3       = var5 * var2;
        var4       = ((var3 / 32768) * (var3 / 32768)) / 128;
        var5       = var3 - ((var4 * ((int32_t)_calibrationData.dig_H1)) / 16);
        var5       = (var5 < 0 ? 0 : var5);
        var5       = (var5 > 419430400 ? 419430400 : var5);
        uint32_t H = (uint32_t)(var5 / 4096);

        return (float)H / 1024.0;
    }

    float BME280::calculatePressure(uint32_t adc_P)
    {
        int64_t var1, var2, var3, var4;

        if (adc_P == 0x800000)
        {
            return NAN;
        }
        adc_P >>= 4;

        var1 = ((int64_t)t_fine) - 128000;
        var2 = var1 * var1 * (int64_t)_calibrationData.dig_P6;
        var2 = var2 + ((var1 * (int64_t)_calibrationData.dig_P5) * 131072);
        var2 = var2 + (((int64_t)_calibrationData.dig_P4) * 34359738368);
        var1 = ((var1 * var1 * (int64_t)_calibrationData.dig_P3) / 256) +
               ((var1 * ((int64_t)_calibrationData.dig_P2) * 4096));
        var3 = ((int64_t)1) * 140737488355328;
        var1 = (var3 + var1) * ((int64_t)_calibrationData.dig_P1) / 8589934592;

        if (var1 == 0)
        {
            return 0; // avoid exception caused by division by zero
        }

        var4 = 1048576 - adc_P;
        var4 = (((var4 * 2147483648) - var2) * 3125) / var1;
        var1 = (((int64_t)_calibrationData.dig_P9) * (var4 / 8192) * (var4 / 8192)) / 33554432;
        var2 = (((int64_t)_calibrationData.dig_P8) * var4) / 524288;
        var4 = ((var4 + var1 + var2) / 256) + (((int64_t)_calibrationData.dig_P7) * 16);

        float P = var4 / 256.0;

        // Convert pascals to mbar
        return P / 100.0;
    }

    void BME280::setSleepMode(void)
    {
        this->m_bus.write8(BME280_ADDRESS, static_cast<uint8_t>(REGISTER::CONTROL),
                           static_cast<uint8_t>(SENSOR_MODE::SLEEP));
    }

} // namespace sensors

#ifdef __cplusplus
}
#endif
