
#ifdef __cplusplus
extern "C" {
#endif


#include "FreeRTOS.h"

#include "BME280.h"

namespace sensors
{
    BME280::BME280(i2c_port_t i2cDriver) :_i2cDriver(i2cDriver) {
        // Soft reset the sensor
        // write8(BME280_REGISTER_SOFTRESET, 0xB6);
        // wait 10ms
        // Wait to finish reading calibration
        // Read calibration data
        // Wait 100ms
    }

    BME280::write8(REGISTER reg, uint8_t value) {
        uint8_t buffer[2];
        buffer[0] = static_cast<uint8_t>(reg);
        buffer[1] = value;
        i2c_master_write_to_device(_i2cDriver, BME280_ADDRESS, &buffer[0], 2, 20 / portTICK_PERIOD_MS)
    }

    BME280::read8(REGISTER reg) {
        uint8_t buffer[1];
        // i2c_master_write_read_device
    }

    BME280::read16(REGISTER reg) {
        uint8_t buffer[2];
    }

    BME280::readS16(REGISTER reg) {
        uint8_t buffer[2];
    }

    BME280::read24(REGISTER reg) {
        uint8_t buffer[3];
    }

    BME280::read16_LE(REGISTER reg) {
        uint8_t buffer[2];
        // letoh16
    }

    BME280::readTemperature(void) {
        int32_t var1, var2;

        int32_t adc_T = read24(TEMPDATA);
        if (adc_T == 0x800000) // value in case temp measurement was disabled
            return NAN;
        adc_T >>= 4;

        var1 = (int32_t)((adc_T / 8) - ((int32_t)_bme280_calib.dig_T1 * 2));
        var1 = (var1 * ((int32_t)_bme280_calib.dig_T2)) / 2048;
        var2 = (int32_t)((adc_T / 16) - ((int32_t)_bme280_calib.dig_T1));
        var2 = (((var2 * var2) / 4096) * ((int32_t)_bme280_calib.dig_T3)) / 16384;

        t_fine = var1 + var2 + t_fine_adjust;

        int32_t T = (t_fine * 5 + 128) / 256;

        return (float)T / 100;
    }

    BME280::readHumidity(void) {
          int64_t var1, var2, var3, var4;

    readTemperature(); // must be done first to get t_fine

    int32_t adc_P = read24(PRESSUREDATA);
    if (adc_P == 0x800000) // value in case pressure measurement was disabled
        return NAN;
    adc_P >>= 4;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)_bme280_calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)_bme280_calib.dig_P5) * 131072);
    var2 = var2 + (((int64_t)_bme280_calib.dig_P4) * 34359738368);
    var1 = ((var1 * var1 * (int64_t)_bme280_calib.dig_P3) / 256) +
            ((var1 * ((int64_t)_bme280_calib.dig_P2) * 4096));
    var3 = ((int64_t)1) * 140737488355328;
    var1 = (var3 + var1) * ((int64_t)_bme280_calib.dig_P1) / 8589934592;

    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }

    var4 = 1048576 - adc_P;
    var4 = (((var4 * 2147483648) - var2) * 3125) / var1;
    var1 = (((int64_t)_bme280_calib.dig_P9) * (var4 / 8192) * (var4 / 8192)) /
            33554432;
    var2 = (((int64_t)_bme280_calib.dig_P8) * var4) / 524288;
    var4 = ((var4 + var1 + var2) / 256) + (((int64_t)_bme280_calib.dig_P7) * 16);

    float P = var4 / 256.0;

    return P;
    }

    BME280::readPressure(void) {
          int32_t var1, var2, var3, var4, var5;

  readTemperature(); // must be done first to get t_fine

  int32_t adc_H = read16(HUMIDDATA);
  if (adc_H == 0x8000) // value in case humidity measurement was disabled
    return NAN;

  var1 = t_fine - ((int32_t)76800);
  var2 = (int32_t)(adc_H * 16384);
  var3 = (int32_t)(((int32_t)_bme280_calib.dig_H4) * 1048576);
  var4 = ((int32_t)_bme280_calib.dig_H5) * var1;
  var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
  var2 = (var1 * ((int32_t)_bme280_calib.dig_H6)) / 1024;
  var3 = (var1 * ((int32_t)_bme280_calib.dig_H3)) / 2048;
  var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
  var2 = ((var4 * ((int32_t)_bme280_calib.dig_H2)) + 8192) / 16384;
  var3 = var5 * var2;
  var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
  var5 = var3 - ((var4 * ((int32_t)_bme280_calib.dig_H1)) / 16);
  var5 = (var5 < 0 ? 0 : var5);
  var5 = (var5 > 419430400 ? 419430400 : var5);
  uint32_t H = (uint32_t)(var5 / 4096);

  return (float)H / 1024.0;
    }

} // namespace sensors

#ifdef __cplusplus
}
#endif