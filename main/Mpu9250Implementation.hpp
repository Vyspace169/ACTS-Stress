/*
 * Mpu9250Implementation.hpp
 *
 *  Created on: 16 nov. 2017
 *      Author: Sander
 */

#pragma once

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/i2c.h"

#define WRITE_BIT              I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT               I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN           0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS          0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                0x0              /*!< I2C ack value */
#define NACK_VAL               0x1              /*!< I2C nack value */
#define I2C_MASTER_SCL_IO      19               /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO      18               /*!< gpio number for I2C master data  */

#define MPU9250_I2C_ADDRESS     0x68

#define I2C_WRITE_BIT           0
#define I2C_READ_BIT            1

// Gyroscale defines
#define GYRO_CONFIG_REG         0x1B
#define GYRO_X_REG              0x43
#define GYRO_Y_REG              0x45
#define GYRO_Z_REG              0x47
#define GYROSCALE_250_DPS       0x00
#define GYROSCALE_500_DPS       0x08
#define GYROSCALE_1000_DPS      0x10
#define GYROSCALE_2000_DPS      0x18

// accelerometer defines
#define ACCEL_CONFIG_REG        0x1C
#define ACCEL_X_REG             0x3B
#define ACCEL_Y_REG             0x3D
#define ACCEL_Z_REG             0x3F
#define ACCELSCALE_2G           0x00
#define ACCELSCALE_4G           0x08
#define ACCELSCALE_8G           0x10
#define ACCELSCALE_16G          0x18

//
#define AK8936_MAG_ADDRESS      0x0C
#define AK8936_MAG_CNTL         0x0A
#define AK8936_MAG_XOUT_L       0x03
#define AK8936_MAG_XOUT_H       0x04
#define AK8936_MAG_YOUT_L       0x05
#define AK8936_MAG_YOUT_H       0x06
#define AK8936_MAG_ZOUT_L       0x07
#define AK8936_MAG_ZOUT_H       0x08

#define AK8936_CONT_MODE_1		0x02
#define AK8936_CONT_MODE_2		0x06


//
#define MPU9250_BYPASS_REG		0x37
#define MPU9250_BYPASS_BIT		0x02

class mpu9250_data{
public:
   unsigned short AccelerometerX;
   unsigned short AccelerometerY;
   unsigned short AccelerometerZ;
   unsigned short GyroscopeX;
   unsigned short GyroscopeY;
   unsigned short GyroscopeZ;
   unsigned short MagnetoX;
   unsigned short MagnetoY;
   unsigned short MagnetoZ;
};


class Mpu9250Implementation {
public:
	Mpu9250Implementation();
	mpu9250_data GetMpu9250Data();
	~Mpu9250Implementation();
private:
protected:
};
