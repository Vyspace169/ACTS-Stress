/*
 * ECGImplementation.hpp
 *
 *  Created on: 22 mrt. 2018
 *      Author: casper.rebholz
 */

#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_err.h"
#include "esp_log.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "Sensor.hpp"

class ECGImplementation: public Sensor{
public:

	ECGImplementation();

   /*!
	 * \brief ecgImplementation DataSize method
	 * \return Datasize of SensorRead method in bytes
	 *
	 * This method returns the datasize of a single
	 * sensorread. This value is given in bytes.
	 *
	 * For the ecg, is is a static value of 4 bytes.
	 */
	int DataSize() override;

   /*!
	 * \brief ECG SensorRead method
	 * \return Pointer to an unsigned short array with data
	 *
	 * This method reads GPIO36 and returns an unsigned
	 * short pointer with the ADC value and an incrementing
	 * sample number.
	 */
   unsigned short* SensorRead() override;

	/*!
	 * \brief ECGImplementation Sleep method
	 *
	 * This method will put the ECG in sleep
	 * mode.
	 */
	void Sleep() override;

private:
   int kernel_size = 5;
   int i = 0;
   int j = 0;
   int sample_number = 0;
   int sample_value[5] = {0};
   int sample_value_filtered = 0; //evt float van maken
   unsigned short ECGData[4];
};





