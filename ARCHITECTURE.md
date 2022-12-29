# Architecture

Based on the suggestion of [this blog](https://matklad.github.io/2021/02/06/ARCHITECTURE.md.html), this document lays out the basic architecture of the project as a guide and starting point for anyone looking to contribute or utilize it.

## Tasks

The logic is split into 2 tasks which run separately on the cores, with a single sync point from the sensor task to the Bluetooth task (BLE) to notify it that data is ready.

### BLE Task

The BLE task initializes the Bluetooth related functionality, releases some memory for functionality which is not used, then waits for the sensor task.

After the sensor task notifies, the BLE task packages up the data into a single BLE advertisement, using the BTHome format.  The class structure for the BTHome data is described separately.

The advertisement is sent a few times, then the task sends the device into deep sleep.

### Sensor Task

The sensor task configures the i2c bus, configures the sensor, gets a sensor reading, puts the sensor to sleep and then notifies the BLE task data is ready for packaging.

## BTHome Classes

The BTHome packet format is primarily managed by the single base class `Sensor`.  This class stores the scaled data, object type and data type and constructs it all into a minimal packet when requested.  Child classes abstract away the data type and object format so a user should only need to select the correct class and provide it with data, then get the payload.

For example, the `TemperatureSensor` has a single constructor which takes the floating point temperature (assumed to be celsius).  it then calls into its parent `SignedIntSensor`, providing the data, but also that the data is a temperature reading, scaled by 100.0.  `UnsignedIntSensor` will finally call to the base class `Sensor`, adding the final bit of metadata, that the packet should be formatted as an unsigned int.
, etc
The whole idea is to prevent the user from needing to know about the internals, just that it's a temperature reading or humidity, etc, with the metadata and scaling all handled invisibly.

## BTHome Advertisement

The BLE advertisement is build via the classes provided in the `bthome` component.  They are generally named in a self explanatory way.  The class will manage inserting standard BLE flags,
optional complete local name, the service UUID and will populate the service data based on the sensors added to it.  If a given sensor has too large a payload to fit, the sensor will not
be added and indication provided to the user.
