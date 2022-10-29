- [BTHome Weather Station](#bthome-weather-station)
  - [Building](#building)
  - [Contributing](#contributing)
  - [References](#references)
  - [Future Work](#future-work)
    - [Encryption](#encryption)
    - [Infrequent Data](#infrequent-data)

# BTHome Weather Station

This is an ESP-IDF based project which collects some sensor data over i2c, encodes it using the [BTHome](https://bthome.io/) format and sends it over Bluetooth.

It is currently working, with the 3 sensors visible in Home Assistant.

## Building

The project uses the [standard ESP-IDF build system](https://docs.espressif.com/projects/esp-idf/en/v4.4.2/esp32/api-guides/build-system.html).
I  personally use the VSCode addon to compile and flash to my device

The SDK config targets a somewhat new revision of an ESP32.  I flash specifically on an Unexpected Maker TinyPcio.

## Contributing

Contributions with new sensors, bug fixes, etc are happily accepted.  The project tries to follow the `git flow` naming, so naming branches `feature/` or `bugfix/`
will help make it clear what its for.

## References

1. [BTHome](https://bthome.io/)
2. [BME280 Datasheet](https://www.mouser.com/datasheet/2/783/BST-BME280-DS002-1509607.pdf)
3. [ESP-IDF BLE API Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html)
4. [ESP-IDF I2C API Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html)

## Future Work

### Encryption

The BTHome format allows for encryption with a pre shared key, using AES-CCM.  The documenting is pretty scarce, with basically only an example Python
script as an example.  For the ESP-IDF, encryption would be handled by the mbed-tls library.  This would require reverse engineering the Python and
replicating it to that library.


### Infrequent Data

Along with this, it could be useful to only send some advertisements infrequently.  I doubt the device name needs to be sent every minute, for example.  I'm
unsure how Home Assistant would treat this though, it may mark the sensor as unavailable.
