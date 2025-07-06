/*BMP280 barometric pressure and temperature sensor C Driver*/
/*Reza Ebrahimi - https://github.com/ebrezadev */
/*Version 1.0*/

#include "bmp280.h"
#include <Wire.h>
#include <math.h>

#define I2C_SPEED   400000

/*writes an array (data[]) of arbitrary size (dataLength) to I2C address (deviceAddress), starting from an internal register address (startRegisterAddress)*/
void bmp280_write_array(uint8_t deviceAddress, uint8_t startRegisterAddress, uint8_t *data, uint8_t dataLength)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(startRegisterAddress);
  for (uint8_t index = 0; index < dataLength; index++)
  {
    Wire.write(data[index]);
  }
  Wire.endTransmission(deviceAddress);
}

/*reads an array (data[]) of arbitrary size (dataLength) from I2C address (deviceAddress), starting from an internal register address (startRegisterAddress)*/
void bmp280_read_array (uint8_t deviceAddress, uint8_t startRegisterAddress, uint8_t *data, uint8_t dataLength)
{
  Wire.beginTransmission(deviceAddress);
  Wire.write(startRegisterAddress);
  Wire.endTransmission(deviceAddress);
  
  Wire.requestFrom(deviceAddress, dataLength, 0);
  while (Wire.available())
  {
    *data = Wire.read();
    data++;
  }
}

/*initiates the I2C peripheral and sets its speed*/
void bmp280_i2c_init()        
{
  Wire.begin();
  Wire.setClock(I2C_SPEED);
}

/*a delay function for milliseconds delay*/
void delay_function (uint32_t delayMS)
{
  delay(delayMS);
}

/*implements a power function (used in altitude calculation)*/
float power_function (float x, float y)
{
  return pow(x,y);
}
