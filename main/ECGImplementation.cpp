/*
 * ECGImplementation.cpp
 *
 *  Created on: 22 mrt. 2018
 *      Author: casper.rebholz
 */

#include "ECGImplementation.hpp"

ECGImplementation::ECGImplementation(){
	memset(ECGData, 0, sizeof(short) * 2 );
}


int ECGImplementation::DataSize() {
	return sizeof(int) * 2;
}

unsigned short* ECGImplementation::SensorRead() {
	sample_value[i] = adc1_get_raw(ADC_ECG);
	sample_number++;

	if(i < kernel_size){
		i++;
	}else {
		i = 0;
	}

	for(int j = 0; j < kernel_size; j++){
		sample_value_filtered += sample_value[j];
	}
	sample_value_filtered = sample_value_filtered / kernel_size;
	j = 0;

	memcpy(ECGData, &sample_value_filtered, sizeof(int));
	memcpy(&ECGData[2], &sample_number, sizeof(int));
	ESP_LOGI("ECGImplementation", "ecg value: %d", sample_value_filtered); // @suppress("Symbol is not resolved")
	return &ECGData[0];

}

void ECGImplementation::Sleep() {

}
