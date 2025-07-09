#include "./mpu9250_i2c.h"

/**
 * Scans for an instance of MPU9250 connected via I2C.
 * 
 * @param i2c I2C port used for scan.
 * 
 * @returns false if sensor is not detected, true if sensor is detected.
 */
bool mpu9250_scan(i2c_inst_t* i2c) {
  uint8_t rxdata;
  return (i2c_read_blocking(i2c, MPU9250_I2C_ADDRESS, &rxdata, 1, false) >= 0);
}

/**
 * Resets MPU9250.
 * 
 * @param i2c I2C port used for writing.
 */
void mpu9250_reset(i2c_inst_t* i2c) {
  // Write 0x80 (only reset bit) to register PWR_MGMT_1.
  uint8_t buffer[2] = {MPU9250_REG_PWR_MGMT_1, 0x80};
  i2c_write_blocking(i2c, MPU9250_I2C_ADDRESS, buffer, 2, false);
  sleep_ms(100);

  // Manually clear reset bit in PWR_MGMT_1 register.
  // (Redundant acoording to datasheet.)
  buffer[1] = 0x00;
  i2c_write_blocking(i2c, MPU9250_I2C_ADDRESS, buffer, 2, false);
  sleep_ms(10);
}

/**
 * Reads data from MPU9250.
 * 
 * @param i2c I2C port used for reading.
 * @param reg Starting register.
 * @param output Output buffer for read data.
 * @param length Length of output buffer.
 */
void mpu9250_read(i2c_inst_t* i2c, const uint8_t reg, uint8_t* output, const uint8_t length) {
  i2c_write_blocking(i2c, MPU9250_I2C_ADDRESS, &reg, 1, true);
  i2c_read_blocking(i2c, MPU9250_I2C_ADDRESS, output, length, false);
}

/**
 * Writes data to MPU9250.
 * 
 * @param i2c I2C port used for writing.
 * @param reg Starting register.
 * @param bytes Input buffer.
 * @param length Length of input buffer.
 */
void mpu9250_write(i2c_inst_t* i2c, const uint8_t reg, const uint8_t* bytes, const uint8_t length) {
  i2c_write_blocking(i2c, MPU9250_I2C_ADDRESS, &reg, 1, true);
  i2c_write_blocking(i2c, MPU9250_I2C_ADDRESS, bytes, length, false);
}

/**
 * Reads raw acceleration values from MPU9250 connected via I2C.
 * 
 * @param i2c I2C port used for reading.
 * @param accel[3] Buffer used to store acceleration values. In order XYZ.
 */
void mpu9250_read_accel_raw(i2c_inst_t* i2c, int16_t accel[3]) {
  // Create 6-byte buffer to store readings.
  uint8_t buffer[6];

  // Custom read function.
  mpu9250_read(i2c, MPU9250_REG_ACCEL_XOUT_H, buffer, 6);

  // Concatenate 2 8-bit values into 1 16-bit value, for XYZ.
  for (uint i = 0; i < 3; ++i) {
    accel[i] = (buffer[i << 1] << 8) + buffer[(i << 1) + 1];
  }
}

/**
 * Reads raw gyroscope values from MPU9250 connected via I2C.
 * 
 * @param i2c I2C port used for reading.
 * @param gyro[3] Buffer used to store gyroscope values. In order XYZ.
 */
void mpu9250_read_gyro_raw(i2c_inst_t* i2c, int16_t gyro[3]) {
  // Create 6-byte buffer to store readings.
  uint8_t buffer[6];

  // Custom read function.
  mpu9250_read(i2c, MPU9250_REG_GYRO_XOUT_H, buffer, 6);

  // Concatenate 2 8-bit values into 1 16-bit value, for XYZ.
  for (uint i = 0; i < 3; ++i) {
    gyro[i] = (buffer[i << 1] << 8) + buffer[(i << 1) + 1];
  }
}

/**
 * Reads raw temperature value from MPU9250 connected via I2C.
 * 
 * @param i2c I2C port used for reading.
 * @param temp Address used to store temperature value.
 */
void mpu9250_read_temp_raw(i2c_inst_t* i2c, int16_t* temp) {
  // Create 2-byte buffer to store readings.
  uint8_t buffer[2];

  // Custom read function.
  mpu9250_read(i2c, MPU9250_REG_TEMP_OUT_H, buffer, 2);

  // Concatenate 2 8-bit values into 1 16-bit value.
  *temp = buffer[0] << 8 + buffer[1];
}