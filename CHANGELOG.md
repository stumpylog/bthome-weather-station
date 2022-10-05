
- [Changelog](#changelog)
  - [vNext](#vnext)
  - [0.0.1](#001)

# Changelog

## vNext

- Implements the class `i2cBus` which wraps the ESP-IDF commands and can be shared with sensors on the same bus
- Implements burst read and write functionality, for reading blocks of data in the same I2C transaction

## 0.0.1

- Initial release of a working BTHome sensor package, using a [TinyPICO](https://www.tinypico.com/) and a BME280 to send temperature, humidity and pressure to Home Assistant
