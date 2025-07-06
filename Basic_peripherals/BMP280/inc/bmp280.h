/*BMP280 barometric pressure and temperature sensor C Driver*/
/*Reza Ebrahimi - https://github.com/ebrezadev */
/*Version 1.0*/

#include "bmp280_defs.h"
#include <stdint.h>

#ifndef __BMP280_H__
#define __BMP280_H__

typedef enum {BMP280_TRUE, BMP280_FALSE} output_status_t;

typedef enum {OVERSAMPLING_0X = 0X00, OVERSAMPLING_1X = 0X01, OVERSAMPLING_2X = 0X02, OVERSAMPLING_4X = 0X03, OVERSAMPLING_8X = 0X04, OVERSAMPLING_16X = 0X05, OVERSAMPLING_ERROR = 0X06} over_sampling_t;

typedef enum {I2C_ADDRESS_1 = 0X76, I2C_ADDRESS_2 = 0X77, BOTH_DETECTED, NONE_DETECTED} i2c_address_t;

typedef enum {T_STANDBY_500US = 0X00, T_STANDBY_62500US, T_STANDBY_125MS, T_STANDBY_250MS, T_STANDBY_500MS, T_STANDBY_1S, T_STANDBY_2S, T_STANDBY_4S, T_STANDBY_ERROR} standby_time_t;

typedef enum {FILTER_OFF = 0X00, FILTER_2X, FILTER_4X, FILTER_8X, FILTER_16X, FILTER_ERROR} iir_filter_t;

typedef enum {MODE_SLEEP = 0X00, MODE_FORCED = 0X01, MODE_NORMAL = 0X03, MODE_ERROR = 0X04} operation_mode_t;

typedef struct 
{
  uint16_t T1; 
  int16_t T2;  
  int16_t T3;  
  uint16_t P1; 
  int16_t P2; 
  int16_t P3;  
  int16_t P4;  
  int16_t P5;  
  int16_t P6;  
  int16_t P7; 
  int16_t P8;  
  int16_t P9; 
} calibration_param_t;

typedef struct
{
  float temperature;
  uint32_t pressure;
  float altitude;
} sensors_t;

i2c_address_t bmp280_init();
output_status_t bmp280_check_id(uint8_t deviceAddress);
i2c_address_t bmp280_check_connected_address();
void bmp280_reset(uint8_t deviceAddress);
void bmp280_set_mode(uint8_t deviceAddress, operation_mode_t operationMode);
void bmp280_set_pressure_accuracy(uint8_t deviceAddress, over_sampling_t osValue);
void bmp280_set_temperature_accuracy(uint8_t deviceAddress, over_sampling_t osValue);
void bmp280_set_standby_time(uint8_t deviceAddress, standby_time_t standbyTime);
void bmp280_set_filter_coefficient(uint8_t deviceAddress, iir_filter_t filterCoefficient);
void bmp280_set(uint8_t deviceAddress, operation_mode_t operationMode, over_sampling_t tempOS, over_sampling_t pressureOS, standby_time_t standbyTime, iir_filter_t filterCoefficient);
void bmp280_set_default(uint8_t deviceAddress);
int32_t bmp280_get_temperature(uint8_t deviceAddress);
uint32_t bmp280_get_pressure(uint8_t deviceAddress);
float bmp280_calculate_altitude_quick(uint8_t deviceAddress, uint32_t barometricPressure);
float bmp280_calculate_altitude_hypsometric(uint8_t deviceAddress, uint32_t barometricPressure, float ambientTemperatureInC);
sensors_t bmp280_get_all(uint8_t deviceAddress);
operation_mode_t bmp280_get_mode(uint8_t deviceAddress);
over_sampling_t bmp280_get_temperature_oversampling(uint8_t deviceAddress);
over_sampling_t bmp280_get_pressure_oversampling(uint8_t deviceAddress);
standby_time_t bmp280_get_standby_time(uint8_t deviceAddress);
iir_filter_t bmp280_get_filter_coefficient(uint8_t deviceAddress);

void bmp280_i2c_init();
void bmp280_read_array(uint8_t deviceAddress, uint8_t startRegisterAddress, uint8_t *data, uint8_t dataLength);
void bmp280_write_array(uint8_t deviceAddress, uint8_t startRegisterAddress, uint8_t *data, uint8_t dataLength);
void delay_function(uint32_t delayMS);
float power_function (float x, float y);

#endif
