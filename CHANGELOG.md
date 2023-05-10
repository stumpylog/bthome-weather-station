
- [Changelog](#changelog)
  - [0.3.0](#030)
  - [0.2.2](#022)
  - [0.2.1](#021)
  - [0.2.0](#020)
  - [0.0.1](#001)

# Changelog

## 0.4.0

- Updated default building to esp-idf v4.4.4
- Added support for bind key encryption [@junchao98](https://github.com/junchao98) in [#3](https://github.com/stumpylog/bthome-weather-station/pull/3)

## 0.3.0

- Advertisements are now built via class and are less handcrafted
- Some initial work for encryption support
- Internal cleanup for C style linkage
- More correctly follow the ESP-IDF build system for components
- Drops support for BTHome v1 format (requires Home Assistant 2022.12 or newer)
- Added CI building against esp-idf v5 (the latest release)

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
