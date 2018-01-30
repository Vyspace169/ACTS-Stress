#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/i2c.h"

#include "Sensor.hpp"

#define WRITE_BIT              		I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT               		I2C_MASTER_READ  /*!< I2C master read */
<<<<<<< HEAD
#define ACK_CHECK_DIS          		0x00              /*!< I2C master will not check ack from slave */
#define ACK_CHECK_EN           		0x01              /*!< I2C master will check ack from slave*/
#define ACK_VAL                		0x00              /*!< I2C ack value */
#define NACK_VAL               		0x01              /*!< I2C nack value */
=======
#define ACK_CHECK_DIS          		0x00             /*!< I2C master will not check ack from slave */
#define ACK_CHECK_EN           		0x01             /*!< I2C master will check ack from slave*/
#define ACK_VAL                		0x00             /*!< I2C ack value */
#define NACK_VAL               		0x01             /*!< I2C nack value */
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
#define I2C_WRITE_BIT           	0x00
#define I2C_READ_BIT            	0x01
#define I2C_TIMEOUT					100

// I2C addresses
#define MPU9250_I2C_ADDRESS     	0x68
#define AK8936_ADDRESS   			0x0C

// MPU9250 registers
#define MPU9250_REG_WHOAMI			0x75
#define MPU9250_REG_GYRO_CONFIG		0x1B
#define MPU9250_REG_GYRO_XL     	0x43
#define MPU9250_REG_GYRO_YL     	0x45
#define MPU9250_REG_GYRO_ZL     	0x47
#define MPU9250_REG_ACCEL_CONFIG	0x1C
#define MPU9250_REG_ACCEL_XL    	0x3B
#define MPU9250_REG_ACCEL_YL    	0x3D
#define MPU9250_REG_ACCEL_ZL    	0x3F
#define MPU9250_REG_BYPASS			0x37
#define MPU9250_REG_PWR_MGMNT_1		0x6B
#define MPU9250_REG_LPF				0x1D
<<<<<<< HEAD
=======
#define MPU9250_REG_INT_EN			0x38
#define MPU9250_REG_WOM_THRES		0x1f
#define MPU9250_REG_INT_CNTRL		0x69
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202

// MPU9250 settings
#define GYROSCALE_250_DPS       	0x00
#define GYROSCALE_500_DPS       	0x08
#define GYROSCALE_1000_DPS      	0x10
#define GYROSCALE_2000_DPS      	0x18
#define ACCELSCALE_2G           	0x00
#define ACCELSCALE_4G           	0x08
#define ACCELSCALE_8G           	0x10
#define ACCELSCALE_16G          	0x18
#define MPU9250_SET_PWR_RESET		0x80
#define MPU9250_SET_BYPASS			0x02
#define MPU9250_SET_LPF_10HZ		0x05
#define MPU9250_SET_SLEEP			0x50
<<<<<<< HEAD

// AK8936 registers
#define AK8936_REG_WHOAMI			0x00
#define AK8936_REG_STATUS1			0x02
#define AK8936_REG_XOUT_L       	0x03
#define AK8936_REG_CNTL1        	0x0A
#define AK8963_REG_CNTL2			0x0B
#define AK8963_REG_SENSE_X			0x10
#define AK8963_REG_SENSE_Y			0x11
#define AK8963_REG_SENSE_Z			0x12

// AK8936 settings
#define AK8936_SET_SING_SAMP		0x01
#define AK8936_SET_CONT_MODE_1		0x02
#define AK8936_SET_CONT_MODE_2		0x06
#define AK8936_SET_FUSE_MODE		0x0F
#define AK8936_SET_16BIT			0x10
#define AK8963_SET_RESET			0x01

void simple_test();
=======
#define WOM_INTERRUPT_EN			0x40
#define WOM_LOGIC_EN				0x80

// AK8936 registers
#define AK8936_REG_WHOAMI			0x00
#define AK8936_REG_STATUS1			0x02
#define AK8936_REG_XOUT_L       	0x03
#define AK8936_REG_CNTL1        	0x0A
#define AK8963_REG_CNTL2			0x0B
#define AK8963_REG_SENSE_X			0x10
#define AK8963_REG_SENSE_Y			0x11
#define AK8963_REG_SENSE_Z			0x12

// AK8936 settings
#define AK8936_SET_SING_SAMP		0x01
#define AK8936_SET_CONT_MODE_1		0x02
#define AK8936_SET_CONT_MODE_2		0x06
#define AK8936_SET_FUSE_MODE		0x0F
#define AK8936_SET_16BIT			0x10
#define AK8963_SET_RESET			0x01

/**
* @file Mpu9250Implementation.hpp
* @data 21 september, 2017
*
* \class Mpu9250Implementation
*
* This class handles the MPU9250 communication. It
* inherrits from the Sensor class and has the same
* methods.
*
*/
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202

class Mpu9250Implementation: public Sensor{
public:
	/*!
	 * \brief Mpu9250Implementation constructor
	 *
	 * This method initializes all used states in this
	 * class. It also intializes the MPU9250 so that all
	 * data can be read with a single method (SensorRead).
	 *
	 * Warning: I2C_NUM_0 is used and should be initialized
	 * before calling this constructor.
	 */
	Mpu9250Implementation();
<<<<<<< HEAD
	int DataSize() override;
	unsigned short* SensorRead() override;
	void Sleep() override;
=======

	/*!
	 * \brief Mpu9250Implementation DataSize method
	 * \return Datasize of SensorRead method in bytes
	 *
	 * This method returns the datasize of a single
	 * sensorread. This value is given in bytes.
	 *
	 * For the MPU9250, is is a static value of 18 bytes.
	 */
	int DataSize() override;

	/*!
	 * \brief Mpu9250Implementation SensorRead method
	 * \return Pointer to an unsigned short array with data
	 *
	 * This method reads data from the MPU9250 using
	 * the I2C bus (I2C_NUM_0). A unsigned short
	 * pointer is returned with 9 axis data.
	 */
	unsigned short* SensorRead() override;

	/*!
	 * \brief Mpu9250Implementation Sleep method
	 *
	 * This method will put the MPU9250 in sleep
	 * mode.
	 */
	void Sleep() override;

	/*!
	 * \brief Mpu9250Implementation destructor
	 *
	 * Empty, not implemented.
	 */
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
	~Mpu9250Implementation() {}
private:
	unsigned short MPUData[9] = {0};
	unsigned short BackupMPUData[9] = {0};
	unsigned char AK8936SenseX;
	unsigned char AK8936SenseY;
	unsigned char AK8936SenseZ;
	bool SensIsInitialized;
	bool MPUIsInitialized;
	bool AKIsInitialized;
<<<<<<< HEAD

   void mpu_init();
   void ak_init();
   void init();
=======
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
protected:
};
