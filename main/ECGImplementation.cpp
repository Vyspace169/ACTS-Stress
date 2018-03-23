/*
 * ECGImplementation.cpp
 *
 *  Created on: 22 mrt. 2018
 *      Author: casper.rebholz
 */

#include "ECGImplementation.hpp"

int ECGImplementation::DataSize() {
	return sizeof(int) * 2;
}

unsigned short* ECGImplementation::SensorRead() {
	this->firstRBuffer = new BinaryBuffer();
	this->secondRBuffer = new BinaryBuffer();
	this->currentR = this->firstRBuffer;
	this->nextR = this->secondRBuffer;
	memset(&ECGData, 0, sizeof(short) * 3 );

	int sample_value[i] = adc1_get_voltage(ADC_ECG);
	int sample_number += 1;

	if(kernel_filled == true){
		for(int j; j < kernel_length; j++){
			sample_value_filtered += sample_value[j];
		}
		sample_value_filtered = sample_value_filtered / kernel_length;
	}

	if(i >= kernel_length){
		0;
		kernel_filled = true;
	}else{
		i++;
	}

	ECGData[0] = sample_value_filtered;
	ECGData[2] = sample_number;

	if(sample_value_filtered > R_PEAK_THRESHOLD){
		if(k >= RBuffer_length){
			k = 0;
		}
		firstRBuffer = sample_value_filtered;
	}else{
		ECGData[3] = 0;
	}

	return &ECGData[0];
}

void ECGImplementation::storeRData(){
	if (!currentR->isFull()){
		currentR->add(in);
	}
	else if(!nextR->isFull()){
		this->swap();
		xEventGroupSetBits(GlobalEventGroupHandle, SensorBufferSdReady);
	}
	//else { critical error }
}

void ECGImplementation::swap(){
	BinaryBuffer * tmp = this->currentR;
	this->currentR = this->nextR;
	this->currentR->writeOnly();
	this->nextR = tmp;
	this->nextR->readOnly();
}

