//Bosch Sensortec BMP280 barometric pressure and temperature sensor
//example implementation for arduino
#include "bmp280.h"

uint8_t deviceAddress;        //device i2c address (which will be detected automatically using bmp280_init function)
sensors_t sensorsData;        //struct container consisting of temperature, pressure and altitude; needed to get full set of data

void setup() {
  Serial.begin(9600);

  deviceAddress = bmp280_init();        //bmp_init() initializes the i2c interface, detects up to two devices connected with their addresses and configures them in default settings

  //next line is not mandatory. using bmp280_set, you can manipulate all the bmp280 settings using one function
  //bmp280_set(deviceAddress, MODE_NORMAL, OVERSAMPLING_4X, OVERSAMPLING_16X, T_STANDBY_250MS, FILTER_16X);
}

void loop() {
  sensorsData = bmp280_get_all(deviceAddress);

  float altitudeHypsometric = bmp280_calculate_altitude_hypsometric(deviceAddress, sensorsData.pressure, sensorsData.temperature);        //second method for a more precise altitude calculation using temperature

  Serial.print("Temperature = ");
  Serial.print(sensorsData.temperature, 2);
  Serial.println(" °C");

  Serial.print("Pressure = ");
  Serial.print(sensorsData.pressure);
  Serial.println(" Pa");

  Serial.print("Altitude (quick) = ");
  Serial.print(sensorsData.altitude);
  Serial.println(" m");

  Serial.print("Altitude (hypsometric) = ");
  Serial.print(altitudeHypsometric, 2);
  Serial.println(" m\n");

  delay(2000);
}
