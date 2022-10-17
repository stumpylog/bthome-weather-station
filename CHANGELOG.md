
- [Changelog](#changelog)
  - [0.2.2](#022)
  - [0.2.1](#021)
  - [0.2.0](#020)
  - [0.0.1](#001)

# Changelog

## 0.2.2

- With the release of Home Assistant 2022.10, bump the sleep time to 5 minutes, which should fix becoming unavailable so quickly.
- Increase advertising interval to 160ms
- Fxes a missing de-init of the BT controller

## 0.2.1

- Fixes an issue with the `i2cBus::write8` method which caused the BME280 to remain in reset
- Fixes the changelog not having been set to the new version

## 0.2.0

- Implements the class `i2cBus` which wraps the ESP-IDF commands and can be shared with sensors on the same bus
- Implements burst read and write functionality, for reading blocks of data in the same I2C transaction

## 0.0.1

- Initial release of a working BTHome sensor package, using a [TinyPICO](https://www.tinypico.com/) and a BME280 to send temperature, humidity and pressure to Home Assistant
