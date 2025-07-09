#ifndef MPU9250_I2C_H
#define MPU9250_I2C_H

#include "hardware/i2c.h"
#include "./mpu9250_defines.h"

extern bool mpu9250_scan(i2c_inst_t* i2c);
extern void mpu9250_reset(i2c_inst_t* i2c);

extern void mpu9250_read(i2c_inst_t* i2c, const uint8_t reg, uint8_t* output, const uint8_t length);
extern void mpu9250_write(i2c_inst_t* i2c, const uint8_t reg, const uint8_t* bytes, const uint8_t length);

#define mpu9250_read_byte(i2c, reg, output) do {\
  mpu9250_read(i2c, reg, output, 1);\
} while (0)

#define mpu9250_write_byte(i2c, reg, byte) do {\
  uint8_t _b = byte;\
  mpu9250_write(i2c, reg, &_b, 1);\
} while(0)

extern void mpu9250_read_accel_raw(i2c_inst_t* i2c, int16_t accel[3]);
extern void mpu9250_read_gyro_raw(i2c_inst_t* i2c, int16_t gyro[3]);
extern void mpu9250_read_temp_raw(i2c_inst_t* i2c, int16_t* temp);

#endif