/*BMP280 barometric pressure and temperature sensor C Driver*/
/*Reza Ebrahimi - https://github.com/ebrezadev */
/*Version 1.0*/

#include "bmp280.h"
#include <stdint.h>
#include <stddef.h>

static calibration_param_t dig1;        /*dig1 and dig2 are based on calibration_param_t struct, which encapsulates all calibration values*/
static calibration_param_t dig2;
static int32_t t_fine;        /*used for pressure compensation, changes with temperature*/

static void bmp280_set_bits_in_register(uint8_t deviceAddress, uint8_t registerAddress, uint8_t fieldData, uint8_t fieldStartBitAddress, uint8_t fieldLength);
static void bmp280_get_calibration(uint8_t deviceAddress, calibration_param_t *dig);
static calibration_param_t *bmp280_which_dig(uint8_t deviceAddress);
static int32_t bmp280_raw_temperature_data(uint8_t deviceAddress);
static int32_t bmp280_raw_pressure_data(uint8_t deviceAddress);
static uint16_t bmp280_read_calibration_word_unsigned(uint8_t deviceAddress, uint8_t startRegisterAddress);
static int16_t bmp280_read_calibration_word_signed(uint8_t deviceAddress, uint8_t startRegisterAddress);
static void bmp280_get_bits_in_register(uint8_t deviceAddress, uint8_t registerAddress, uint8_t *fieldData, uint8_t fieldStartBitAddress, uint8_t fieldLength);

/*initializer, detects the connected chips automatically and resets all of them. returns the detected addresses (if any), 
 * and automatically gets the calibration data for further calculations. also sets the default values.
 */
i2c_address_t bmp280_init()
{
  bmp280_i2c_init();

  i2c_address_t checkConnection = bmp280_check_connected_address();       /*automatically detects all connected bmp280 chips and their i2c address*/

  if (checkConnection == BOTH_DETECTED)       /*reseting, getting calibration data and setting default values in all connected chips*/
  {
    bmp280_reset(I2C_ADDRESS_1);
    bmp280_reset(I2C_ADDRESS_2);

    delay_function(STARTUP_DELAY_IN_MS);

    bmp280_get_calibration(I2C_ADDRESS_1, &dig1);
    bmp280_get_calibration(I2C_ADDRESS_2, &dig2);

    bmp280_set_default(I2C_ADDRESS_1);
    bmp280_set_default(I2C_ADDRESS_2);

    return BOTH_DETECTED;
  }
  else if (checkConnection == I2C_ADDRESS_1)
  {
    bmp280_reset(I2C_ADDRESS_1);
    
    delay_function(STARTUP_DELAY_IN_MS);
    
    bmp280_get_calibration(I2C_ADDRESS_1, &dig1);
    
    bmp280_set_default(I2C_ADDRESS_1);

    return I2C_ADDRESS_1;
  }
  else if (checkConnection == I2C_ADDRESS_2)
  {
    bmp280_reset(I2C_ADDRESS_2);
    
    delay_function(STARTUP_DELAY_IN_MS);
    
    bmp280_get_calibration(I2C_ADDRESS_2, &dig2);
    
    bmp280_set_default(I2C_ADDRESS_2);

    return I2C_ADDRESS_2;
  }
  else
  {
    return NONE_DETECTED;
  }
}

/*checks chip id to see if it really is a bmp280 module with correct wiring and address*/
output_status_t bmp280_check_id(uint8_t deviceAddress)
{
  uint8_t data;
  bmp280_read_array (deviceAddress, ID_ADDRESS, &data, 1);

  if (data == BMP280_DEFAULT_CHIP_ID)
  {
    return BMP280_TRUE;
  }
  else
  {
    return BMP280_FALSE;
  }
}

/*discovers which of two i2c addresses are used, if any or both*/
i2c_address_t bmp280_check_connected_address()
{
  output_status_t id1 = bmp280_check_id(I2C_ADDRESS_1);
  output_status_t id2 = bmp280_check_id(I2C_ADDRESS_2);

  if ((id1 == BMP280_TRUE) && (id2 == BMP280_TRUE))
  {
    return BOTH_DETECTED;
  }
  else if (id1 == BMP280_TRUE)
  {
    return I2C_ADDRESS_1;
  }
  else if (id2 == BMP280_TRUE)
  {
    return I2C_ADDRESS_2;
  }
  else
  {
    return NONE_DETECTED;
  }
}

/*soft resets bm280 using special reset register*/
void bmp280_reset(uint8_t deviceAddress)
{
  uint8_t data = BMP280_RESET_VALUE;
  bmp280_write_array(deviceAddress, RESET_ADDRESS, &data, 1);
}

/*setting bmp280 mode, NORMAL_MODE, FORCED_MODE, SLEEP_MODE*/
void bmp280_set_mode(uint8_t deviceAddress, operation_mode_t operationMode)
{
  bmp280_set_bits_in_register(deviceAddress, CTRL_MEAS_ADDRESS, operationMode, MODE_BIT, MODE_LENGTH);
}

/*setting pressure oversampling from 0 (skip) to 16x*/
void bmp280_set_pressure_accuracy(uint8_t deviceAddress, over_sampling_t osValue)
{
  bmp280_set_bits_in_register(deviceAddress, CTRL_MEAS_ADDRESS, osValue, OSRS_P_BIT, OSRS_P_LENGTH);
}

/*setting temperature oversampling from 0 (skip) to 16x*/
void bmp280_set_temperature_accuracy(uint8_t deviceAddress, over_sampling_t osValue)
{
  bmp280_set_bits_in_register(deviceAddress, CTRL_MEAS_ADDRESS, osValue, OSRS_T_BIT, OSRS_T_LENGTH);
}

/*sets standby time between measurements in normal mode. lower standby time means higher power consumption*/
void bmp280_set_standby_time(uint8_t deviceAddress, standby_time_t standbyTime)
{
  bmp280_set_bits_in_register(deviceAddress, CONFIG_ADDRESS, standbyTime, T_SB_BIT, T_SB_LENGTH);
}

/*sets low pass internal filter coefficient for bmp280. used in noisy environments*/
void bmp280_set_filter_coefficient(uint8_t deviceAddress, iir_filter_t filterCoefficient)
{
  bmp280_set_bits_in_register(deviceAddress, CONFIG_ADDRESS, filterCoefficient, FILTER_BIT, FILTER_LENGTH);
}

/*one function to completely set up bmp280 mode, temperature and pressure oversampling, normal mode stadby time and IIR filter coefficient*/
void bmp280_set(uint8_t deviceAddress, operation_mode_t operationMode, over_sampling_t tempOS, over_sampling_t pressureOS, standby_time_t standbyTime, iir_filter_t filterCoefficient)
{
  bmp280_set_mode(deviceAddress, MODE_SLEEP);
  delay_function(50);
  bmp280_set_pressure_accuracy(deviceAddress, pressureOS);
  bmp280_set_temperature_accuracy(deviceAddress, tempOS);
  bmp280_set_standby_time(deviceAddress, standbyTime);
  bmp280_set_filter_coefficient(deviceAddress, filterCoefficient);
  bmp280_set_mode(deviceAddress, operationMode);
}

/*sets bmp280 to defaults*/
void bmp280_set_default(uint8_t deviceAddress)
{
  bmp280_set_mode(deviceAddress, MODE_SLEEP);
  delay_function(50);
  bmp280_set_pressure_accuracy(deviceAddress, PRESSURE_OVERSAMPLING_DEFAULT);
  bmp280_set_temperature_accuracy(deviceAddress, TEMPERATURE_OVERSAMPLING_DEFAULT);
  bmp280_set_standby_time(deviceAddress, STANDBY_TIME_DEFAULT);
  bmp280_set_filter_coefficient(deviceAddress, FILTER_DEFAULT);
  bmp280_set_mode(deviceAddress, MODE_DEFAULT);
}

/*reads temperature value from internal bmp280 registers in centigrade*/
int32_t bmp280_get_temperature(uint8_t deviceAddress)
{
  calibration_param_t *dig = bmp280_which_dig(deviceAddress);       /*automatically relating device address and calibration data*/

  int32_t adc_T = bmp280_raw_temperature_data(deviceAddress);      /*per Bosch guideline*/
  int32_t var1, var2, T;

  var1 = ((((adc_T >> 3) - ((int32_t)dig->T1 << 1))) * ((int32_t)dig->T2)) >> 11;
  var2 = (((((adc_T >> 4) - ((int32_t)dig->T1)) * ((adc_T >> 4) - ((int32_t)dig->T1))) >> 12) * ((int32_t)dig->T3)) >> 14;

  t_fine = var1 + var2;
  T = (t_fine * 5 + 128) >> 8;
  return T;
}

/*reads pressure value from internal bmp280 registers in pascal*/
uint32_t bmp280_get_pressure(uint8_t deviceAddress)
{
  calibration_param_t *dig = bmp280_which_dig(deviceAddress);

  int32_t adc_P = bmp280_raw_pressure_data(deviceAddress);
  int32_t var1, var2;
  uint32_t p;

  var1 = (((int32_t) t_fine) / 2) - (int32_t) 64000;
  var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t) dig->P6);
  var2 = var2 + ((var1 * ((int32_t) dig->P5)) * 2);
  var2 = (var2 / 4) + (((int32_t) dig->P4) * 65536);
  var1 = (((dig->P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8) + ((((int32_t) dig->P2) * var1) / 2)) / 262144;
  var1 = ((((32768 + var1)) * ((int32_t) dig->P1)) / 32768);
  p = (uint32_t)(((int32_t)(1048576 - adc_P) - (var2 / 4096)) * 3125);

  /* Avoid exception caused by division with zero */
  if (var1 != 0)
  {
    /* Check for overflows against UINT32_MAX/2; if pres is left-shifted by 1 */
    if (p < 0x80000000)
    {
      p = (p << 1) / ((uint32_t) var1);
    }
    else
    {
      p = (p / (uint32_t) var1) * 2;
    }
    var1 = (((int32_t) dig->P9) * ((int32_t) (((p / 8) * (p / 8)) / 8192))) / 4096;
    var2 = (((int32_t) (p / 4)) * ((int32_t) dig->P8)) / 8192;
    p = (uint32_t) ((int32_t)p + ((var1 + var2 + dig->P7) / 16));
  }
  else
  {
    p = 0;
  }
  return p;
}

/*calculates altitude from barometric pressure without temperature as an argument*/
float bmp280_calculate_altitude_quick(uint8_t deviceAddress, uint32_t barometricPressure)
{
  float altitudeFloat;

  altitudeFloat = 44307.69396 * (1 - 0.111555816 * power_function(barometricPressure, 0.190284));       /*calculating altitude from barometric formula*/

  return altitudeFloat;
}

/*calculates altitude from barometric pressure and temperature as arguments*/
float bmp280_calculate_altitude_hypsometric(uint8_t deviceAddress, uint32_t barometricPressure, float ambientTemperatureInC)
{
  float altitudeFloat;

  altitudeFloat = ((ambientTemperatureInC + 273.15) * (power_function((float)SEA_LEVEL_PRESSURE / (float)barometricPressure, (float)1 / 5.257) - 1)) / 0.0065;

  return altitudeFloat;
}

/*returns a complete set of sensor readings and altitude calculation (quick). if bmp280 is in sleep mode, gets data by setting bmp280 to forced mode then back to sleep mode*/
sensors_t bmp280_get_all(uint8_t deviceAddress)
{
  sensors_t sensors_value;
  
  operation_mode_t currentMode = bmp280_get_mode(deviceAddress);

  if(currentMode == MODE_SLEEP)       /*gets a complete reading of sensors even if the chip is in sleep mode*/
  {
    bmp280_set_mode(deviceAddress, MODE_FORCED);
    delay_function(10);
  }
  
  sensors_value.temperature = (float)bmp280_get_temperature(deviceAddress) / 100;
  sensors_value.pressure = bmp280_get_pressure(deviceAddress);
  sensors_value.altitude = bmp280_calculate_altitude_quick(deviceAddress, sensors_value.pressure);
  
  if(currentMode == MODE_SLEEP)
  {
    bmp280_set_mode(deviceAddress, MODE_SLEEP);
  }
  
  return sensors_value;
}

/*returns bmp280 mode of operation: sleep, normal or forced*/
operation_mode_t bmp280_get_mode(uint8_t deviceAddress)
{
  operation_mode_t operationMode;
  uint8_t readMode;

  bmp280_get_bits_in_register(deviceAddress, CTRL_MEAS_ADDRESS, &readMode, MODE_BIT, MODE_LENGTH);

  switch (readMode)
  {
    case 0X00:
      operationMode = MODE_SLEEP;
      break;
    case 0X01:
      operationMode = MODE_FORCED;
      break;
    case 0X02:
      operationMode = MODE_FORCED;
      break;
    case 0X03:
      operationMode = MODE_NORMAL;
      break;
    default:
      operationMode = MODE_ERROR;
      break;
  }
  return operationMode;
}

/*returns the current temperature oversampling*/
over_sampling_t bmp280_get_temperature_oversampling(uint8_t deviceAddress)
{
  over_sampling_t tempOversampling;
  uint8_t readOS;

  bmp280_get_bits_in_register(deviceAddress, CTRL_MEAS_ADDRESS, &readOS, OSRS_T_BIT, OSRS_T_LENGTH);

  if (readOS < OVERSAMPLING_ERROR)
  {
    tempOversampling = (over_sampling_t)readOS;
  }
  else
  {
    tempOversampling = OVERSAMPLING_ERROR;
  }
  return tempOversampling;
}

/*returns the current pressure oversampling*/
over_sampling_t bmp280_get_pressure_oversampling(uint8_t deviceAddress)
{
  over_sampling_t pressureOversampling;
  uint8_t readOS;

  bmp280_get_bits_in_register(deviceAddress, CTRL_MEAS_ADDRESS, &readOS, OSRS_P_BIT, OSRS_P_LENGTH);

  if (readOS < OVERSAMPLING_ERROR)
  {
    pressureOversampling = (over_sampling_t)readOS;
  }
  else
  {
    pressureOversampling = OVERSAMPLING_ERROR;
  }

  return pressureOversampling;
}

/*returns the current standby time (for normal mode)*/
standby_time_t bmp280_get_standby_time(uint8_t deviceAddress)
{
  standby_time_t standbyTime;
  uint8_t readTime;

  bmp280_get_bits_in_register(deviceAddress, CONFIG_ADDRESS, &readTime, T_SB_BIT, T_SB_LENGTH);

  if (readTime < T_STANDBY_ERROR)
  {
    standbyTime = (standby_time_t)readTime;
  }
  else
  {
    standbyTime = T_STANDBY_ERROR;
  }
  return standbyTime;
}

/*returns the current IIR filter coefficient*/
iir_filter_t bmp280_get_filter_coefficient(uint8_t deviceAddress)
{
  iir_filter_t filterCoefficient;
  uint8_t readFilter;

  bmp280_get_bits_in_register(deviceAddress, CONFIG_ADDRESS, &readFilter, FILTER_BIT, FILTER_LENGTH);

  if (readFilter < FILTER_ERROR)
  {
    filterCoefficient = (iir_filter_t)readFilter;
  }
  else
  {
    filterCoefficient = FILTER_ERROR;
  }

  return filterCoefficient;
}

/*raw reading of temperature registers, uncompensated*/
static int32_t bmp280_raw_temperature_data(uint8_t deviceAddress)
{
  uint8_t temp[3];
  bmp280_read_array(deviceAddress, TEMP_MSB, temp, 3);
  return (int32_t)((((uint32_t)temp[0]) << 12) + (((uint32_t)temp[1]) << 4) + (((uint32_t)temp[2]) >> 4));
}

/*raw reading of pressure registers, uncompensated*/
static int32_t bmp280_raw_pressure_data(uint8_t deviceAddress)
{
  uint8_t pressure[3];
  bmp280_read_array(deviceAddress, PRESSURE_MSB, pressure, 3);
  return (int32_t)((((uint32_t)pressure[0]) << 12) + (((uint32_t)pressure[1]) << 4) + (((uint32_t)pressure[2]) >> 4));
}

/*a maximum umber of two bmp280 chips could be used on one i2c line
* this function returns a pointer to the complete struct of calibrations parameters for one chip,
* base on device address
*/
static calibration_param_t *bmp280_which_dig(uint8_t deviceAddress)
{
  switch (deviceAddress)
  {
    case I2C_ADDRESS_1:
      return &dig1;
      break;
    case I2C_ADDRESS_2:
      return &dig2;
      break;
    default:
      return NULL;
      break;
  }
}

/*extracting calibration data in chip's "non volatile memory". we need to do this only once for each chip*/
static void bmp280_get_calibration(uint8_t deviceAddress, calibration_param_t *dig)
{
  dig->T1 = bmp280_read_calibration_word_unsigned(deviceAddress, T1_ADDRESS);
  dig->T2 = bmp280_read_calibration_word_signed(deviceAddress, T2_ADDRESS);
  dig->T3 = bmp280_read_calibration_word_signed(deviceAddress, T3_ADDRESS);
  dig->P1 = bmp280_read_calibration_word_unsigned(deviceAddress, P1_ADDRESS);
  dig->P2 = bmp280_read_calibration_word_signed(deviceAddress, P2_ADDRESS);
  dig->P3 = bmp280_read_calibration_word_signed(deviceAddress, P3_ADDRESS);
  dig->P4 = bmp280_read_calibration_word_signed(deviceAddress, P4_ADDRESS);
  dig->P5 = bmp280_read_calibration_word_signed(deviceAddress, P5_ADDRESS);
  dig->P6 = bmp280_read_calibration_word_signed(deviceAddress, P6_ADDRESS);
  dig->P7 = bmp280_read_calibration_word_signed(deviceAddress, P7_ADDRESS);
  dig->P8 = bmp280_read_calibration_word_signed(deviceAddress, P8_ADDRESS);
  dig->P9 = bmp280_read_calibration_word_signed(deviceAddress, P9_ADDRESS);
}

/*only used in get_calibration function*/
static uint16_t bmp280_read_calibration_word_unsigned(uint8_t deviceAddress, uint8_t startRegisterAddress)
{
  uint8_t dataLSB, dataMSB;
  bmp280_read_array (deviceAddress, startRegisterAddress, &dataLSB, 1);
  bmp280_read_array (deviceAddress, startRegisterAddress + 1, &dataMSB, 1);
  return (uint16_t)((((uint16_t)dataMSB) << 8) + (uint16_t)dataLSB);
}

/*only used in get_calibration function*/
static int16_t bmp280_read_calibration_word_signed(uint8_t deviceAddress, uint8_t startRegisterAddress)
{
  uint8_t dataLSB, dataMSB;
  bmp280_read_array (deviceAddress, startRegisterAddress, &dataLSB, 1);
  bmp280_read_array (deviceAddress, startRegisterAddress + 1, &dataMSB, 1);
  return (int16_t)((((int16_t)dataMSB) << 8) + (int16_t)dataLSB);
}

/*sets a field of bits in a register*/
static void bmp280_set_bits_in_register(uint8_t deviceAddress, uint8_t registerAddress, uint8_t fieldData, uint8_t fieldStartBitAddress, uint8_t fieldLength)
{
  uint8_t currentRegisterValue;
  uint8_t newRegisterValue;

  bmp280_read_array(deviceAddress, registerAddress, &currentRegisterValue, 1);

  currentRegisterValue &= (((~0) << (fieldStartBitAddress + fieldLength)) | (~(~(0) << fieldStartBitAddress)));
  newRegisterValue = currentRegisterValue | (fieldData << fieldStartBitAddress);

  bmp280_write_array(deviceAddress, registerAddress, &newRegisterValue, 1);
}

/*gets a field of bits from a register*/
static void bmp280_get_bits_in_register(uint8_t deviceAddress, uint8_t registerAddress, uint8_t *fieldData, uint8_t fieldStartBitAddress, uint8_t fieldLength)
{
  uint8_t registerValue;

  bmp280_read_array(deviceAddress, registerAddress, &registerValue, 1);

  registerValue &= ~(((~0) << (fieldStartBitAddress + fieldLength)) | (~(~(0) << fieldStartBitAddress)));
  *fieldData = registerValue >> fieldStartBitAddress;
}
