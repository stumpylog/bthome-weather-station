- [BTHome Weather Station](#bthome-weather-station)
  - [Building](#building)
  - [Contributing](#contributing)
  - [References](#references)
  - [Future Work](#future-work)
    - [BLE Advertisement Building](#ble-advertisement-building)
      - [Improved Building](#improved-building)
      - [Splitting Packets](#splitting-packets)
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
1. [BME280 Datasheet](https://www.mouser.com/datasheet/2/783/BST-BME280-DS002-1509607.pdf)
2. [ESP-IDF BLE API Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/bluetooth/esp_gap_ble.html)
3. [ESP-IDF I2C API Documentation](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/i2c.html)

## Future Work

### BLE Advertisement Building

#### Improved Building

A lot of the BLE packet is handcrafted indexing.  This includes the device name, and quite a lot of very explicit indexing into a single
array of bytes.  That's not great, and I'm sure it could be improved.  Perhaps a packed structure or a management class with
interfaces to set certain parts of the packet (or not)?

#### Splitting Packets

Currently, all the data read fits into a single BLE advertisement packet (31 bytes of data).  As an improvement, some sort of interface should
exist to break the data across multiple packets if necessary.  This might place information like the device name into one advertisement and
the actual readings into a second.

#### Infrequent Data

Along with this, it could be useful to only send some advertisements infrequently.  I doubt the device name needs to be sent every minute, for example.
