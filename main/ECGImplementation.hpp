/*
 * ECGImplementation.hpp
 *
 *  Created on: 22 mrt. 2018
 *      Author: casper.rebholz
 */

#pragma once
#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "Sensor.hpp"
#include "Systemerrors.hpp"
#include "Errorhandler.hpp"
#include "BinaryBuffer.hpp"

#define TAG_ECG "ECG"

class ECGImplementation: public Sensor{
public:
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
	 * \brief swap method
	 *
	 * This method swaps the ping pong buffer to the
	 * next one.
	 */
	void swap();

	/*!
	 * \brief storeData method
	 * \param in SampleData structure
	 *
	 * This method stores potential R-peaks in the
	 * currently used Rbuffer.
	 */
	void storeRData(RData in);

   int RBuffer_length = 500;
   BinaryBuffer * firstRBuffer;
   BinaryBuffer * secondRBuffer;

private:
   int kernel_length = 5;
   int i;
   int k;
   bool kernel_filled;
   int sample_number;
   int sample_value[5];
   int sample_value_filtered; //evt float van maken
   unsigned short ECGData[2];

   BinaryBuffer * currentR;
   BinaryBuffer * nextR;

};





