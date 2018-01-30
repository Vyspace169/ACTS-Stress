#pragma once
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "bmp280.h"
#include "Sensor.hpp"
#include "Systemerrors.hpp"
#include "Errorhandler.hpp"

#define TAG_BMP280 "BMP280"

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

enum class BMP280ErrTypes{
   NO_ERROR,
   INIT_ERROR_1,
   INIT_ERROR_2,
   INIT_ERROR_3,
   INIT_ERROR_4,
   INIT_ERROR_5,
   INIT_ERROR_6,
};
<<<<<<< HEAD
class Bmp280Implementation: public Sensor{
public:
   Bmp280Implementation();
   int DataSize() override;
   unsigned short* SensorRead() override;
   void Sleep() override {}
=======

/**
* @file Bmp280Implementation.hpp
* @data 21 september, 2017
*
* \class Bmp280Implementation
*
* This class handles the BMP280 communication. It
* inherrits from the Sensor class and has the same
* methods.
*
*/
class Bmp280Implementation: public Sensor{
public:

	/*!
	 * \brief Bmp280Implementation constructor
	 *
	 * This method initializes all used states in this
	 * class. It also intializes the BMP280 so that all
	 * data can be read with a single method (SensorRead).
	 *
	 * Warning: I2C_NUM_0 is used and should be initialized
	 * before calling this constructor.
	 */
   Bmp280Implementation();

   /*!
   	 * \brief Bmp280Implementation DataSize method
   	 * \return Datasize of SensorRead method in bytes
   	 *
   	 * This method returns the datasize of a single
   	 * sensorread. This value is given in bytes.
   	 *
   	 * For the BMP280, is is a static value of 8 bytes.
   	 */
   int DataSize() override;

   /*!
   	 * \brief Bmp280Implementation SensorRead method
   	 * \return Pointer to an unsigned short array with data
   	 *
   	 * This method reads data from the BMP280 using
   	 * the I2C bus (I2C_NUM_0). A unsigned short
   	 * pointer is returned with temparature and
   	 * airpressure data.
   	 */
   unsigned short* SensorRead() override;

   /*!
   	 * \brief Bmp280Implementation Sleep method
   	 *
   	 * Empty, not implemented.
   	 */
   void Sleep() override {}

   /*!
   	 * \brief Bmp280Implementation destructor
   	 *
   	 * Empty, not implemented.
   	 */
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
   ~Bmp280Implementation() {}
private:
   struct bmp280_t bmp280_com_functions;
   unsigned short BMPData[4];
   unsigned short BackupBMPData[4];
   bool IsInitialized;
   BMP280ErrTypes error_type = BMP280ErrTypes::NO_ERROR;
protected:
};

//void i2c_master_init(); // not important for bmp280
s8 BMP280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BMP280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BMP280_delay_msek(u32 msek);
void task_bmp280_normal_mode(void *ignore);
