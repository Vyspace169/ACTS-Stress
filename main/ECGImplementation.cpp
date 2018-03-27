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
	i++;

	if(i == kernel_size){
		i = 0;
	}

	if(sample_number > BINARY_BUFFER_SIZE){
		sample_number = 0;
	}

	for(int j = 0; j < kernel_size; j++){
		sample_value_filtered += sample_value[j];
	}
	sample_value_filtered = sample_value_filtered / kernel_size;
	j = 0;

	memcpy(ECGData, &sample_value_filtered, sizeof(int));
	memcpy(&ECGData[1], &sample_number, sizeof(int));
	//ESP_LOGW("ECG"," sample_value_filtered %d", sample_value_filtered);
	sample_value_filtered = 0;

	return &ECGData[0];

}

void ECGImplementation::Sleep() {

}
