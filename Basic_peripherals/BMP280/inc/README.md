# BMP280 Barometric Pressure and Temperature Sensor C Driver
* version 1.0
* Reza Ebrahimi

BMP280 is an absolute barometric pressure sensor especially designed by Bosch Sensortec for mobile applications. It consists of a Piezo-resistive pressure sensing element and a mixed-signal ASIC. The ASIC performs A/D conversions and provides the conversion results and sensor specific compensation data through a digital interface. This library is written in **portable C**, and is **MCU independent**. In order to implement it to your MCU of choice, you need to manipulate functions inside bmp280_low_level.c file (I2C configurations, delay function and math operations) and leave other files as they are.

BMP280 C library abstracts away the internals of the hardware, using high level functions (description below). BMP280 is capable of interfacing with SPI and I2C; Only I2C is implemented in this library.

This library is based on the 'BMP280 Digital Pressure Sensor Datsheet' V. 1.23.

## GENERAL INFORMATION

BMP280 consists of two sensors on one module: temperature and barometric pressure. The barometric pressure value needs to be calibrated and compensated using temperature data, so a reading of temperature must be done prior to a pressure reading.

The BMP280 has three modes of operation (Normal, Forced and Sleep), an optional IIR filter for noisy environments, optional timing in normal mode (continuous reading) and optional oversampling.

It can also be used as an altimeter based on barometric pressure. Two functions are provided for this purpose.

An added feature is automatic detection of BMP280 sensors on I2C bus and their addresses.

## HOW TO USE

As mentioned earlier, first thing to do is implementing low level settings inside bmp280_low_level.c which consists of I2C settings, a delay function and math calculations for altitude. In order to use, the first function to call is bmp280_init, which initializes I2C interface, detects BMP280 sensors and their addresses, soft resets and loads them with default settings, reads the non volatile memory of BMP280 and stores their data for temperature and pressure value compensations, and returns the detected addresses.

In the next step, BMP280 settings could be changed using the bmp280_set(...) function, as well as mode of operation. Both are optional and not mandatory for a sensor reading, because the module is loaded with default settings through bmp280_init() function.
A full sensors reading could be done with:

**sensors_t bmp280_get_all(uint8_t deviceAddress)**

The return value will be of custom type struct sensors_t, which consists of temperature, pressure and altitude. Device address is detected beforehand, using bmp280_init. There are individual functions provided for custom readings and code.


